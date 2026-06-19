import os
import sys
import json
from typing import Any, Dict, List, Optional, Sequence, Type, TYPE_CHECKING,Literal

from llama_index.core.indices.property_graph.transformations import (
    ImplicitPathExtractor,
)
from llama_index.core import PropertyGraphIndex
from llama_index.core.indices.property_graph import ImplicitPathExtractor
from llama_index.core.schema import TextNode
from llama_index.core.graph_stores.types import Relation, KG_RELATIONS_KEY
from neo4j import GraphDatabase
from llama_index.graph_stores.neo4j import Neo4jPropertyGraphStore
from llama_index.embeddings.databricks import DatabricksEmbedding
from dotenv import load_dotenv
if os.getcwd() not in sys.path: sys.path.append(os.getcwd())
# Load the .env file
load_dotenv()
from dataclasses import dataclass, field
from enum import Enum, auto
import tiktoken
# NEO4J_USERNAME=ad
# NEO4J_PASSWORD=Saigon123456
# NEO4J_URI=neo4j://104.41.147.37:7687
# CODE_ELEMENTS_PATH=AI-CodingAssistant\data\code_elements.json
# OPENAI_API_KEY=


# Set up the DatabricksEmbedding class with the required model, API key and serving endpoint
# os.environ["DATABRICKS_TOKEN"] = ''
# os.environ["DATABRICKS_SERVING_ENDPOINT"] = 'https://adb-379144824042062.2.azuredatabricks.net/serving-endpoints'

embed_model = DatabricksEmbedding(model=os.environ.get('MODEL_EMBEDDING_NAME'))

def delete_all(tx):
    tx.run("MATCH (n) DETACH DELETE n")

def get_driver():
    return GraphDatabase.driver(os.environ.get('NEO4J_URI'), auth=(os.environ.get('NEO4J_USERNAME'), os.environ.get('NEO4J_PASSWORD')))
def get_graph_store():
    return Neo4jPropertyGraphStore(
        username=os.environ.get('NEO4J_USERNAME'),
        password=os.environ.get('NEO4J_PASSWORD'),
        url=os.environ.get('NEO4J_URI'),
    )
class NodeRelationship(str, Enum):
    """Node relationships used in `BaseNode` class.

    Attributes:
        SOURCE: The node is the source document.
        PREVIOUS: The node is the previous node in the document.
        NEXT: The node is the next node in the document.
        PARENT: The node is the parent node in the document.
        CHILD: The node is a child node in the document.

    """

    SOURCE = auto()
    PREVIOUS = auto()
    NEXT = auto()
    PARENT = auto()
    CHILD = auto()
    CALL = auto()
    CALLED_BY = auto()

def get_node_rel_string(relationship: NodeRelationship) -> str:
    return str(relationship).split(".")[-1]

@dataclass
class CodeNode:
    id: int
    text: str
    type: str
    children_types: List[str]
    origin_file: Optional[str] = None
    parent_id: Optional[int] = None
    parent_type: Optional[str] = None
    relationships: Dict[str, List[int]] = field(default_factory=lambda: {
        'calls': [],           # Function calls
        'called_by': [],       # Functions that call this
    })
    chunks: Optional[List['CodeNode']] = None

def truncate_text(text: str, model: str = "text-embedding-3-small", max_tokens: int = 2000):
    encoder = tiktoken.encoding_for_model(model)
    tokens = encoder.encode(text)
    return encoder.decode(tokens[:max_tokens])

def create_node_and_relation(codeNodes: List[Dict]) -> List[TextNode]:
    """
    Create nodes and their relationships from a list of code node dictionaries.
    
    Args:
        codeNodes (List[Dict]): List of code node dictionaries containing text and chunk information
        
    Returns:
        List[TextNode]: List of created nodes with their relationships
    """
    nodes = {}
    
    # First, create all nodes
    for node in codeNodes:
        parent_node = TextNode(
            text=truncate_text(node.get('text', '')),
            id_=f"{node.get('id')}",
            metadata={
                "code_type": node.get('type'),
                "origin_file": node.get('origin_file'),
                "KG_RELATIONS_KEY":[]
            },
        )
        nodes[parent_node.id_] = parent_node
        
        # Process child nodes if chunks exist
        child_nodes = []
        if node.get('chunks'):
            for chunk in node['chunks']:
                child_node = TextNode(
                    text=truncate_text(chunk.get('text', '')),
                    id_=f"{chunk.get('id')}",
                    metadata={
                        "code_type": chunk.get('type'),
                        "origin_file": chunk.get('origin_file'),
                    },
                )
                child_node.metadata[KG_RELATIONS_KEY]=[Relation(
                                        source_id=parent_node.id_,
                                        target_id=child_node.id_,
                                        label=get_node_rel_string(NodeRelationship.PARENT),
                                    )]
                nodes[child_node.id_] = child_node
                child_nodes.append(child_node)
            
            # Create relationships between consecutive child nodes
            for i in range(len(child_nodes) - 1):
                child_nodes[i].metadata[KG_RELATIONS_KEY].extend([Relation(
                                                            source_id=child_nodes[i].id_,
                                                            target_id=child_nodes[i + 1].id_,
                                                            label=get_node_rel_string(NodeRelationship.NEXT),
                                                            )])
    
    # Second, establish relationships from 'relationships' key
    for node in codeNodes:
        current_node = nodes.get(f"{node.get('id')}")
        if not current_node:
            continue
        
        relationships = node.get('relationships', {})
        
        # Process 'calls' relationships
        for target_id in relationships.get('calls', []):
            if str(target_id) in nodes:
                current_node.metadata.setdefault(KG_RELATIONS_KEY, []).extend([Relation(
                                                            source_id=current_node.id_,
                                                            target_id=str(target_id),
                                                            label=get_node_rel_string(NodeRelationship.CALL),
                                                            )])
        
        # Process 'called_by' relationships
        for source_id in relationships.get('called_by', []):
            if str(source_id) in nodes:
                current_node.metadata.setdefault(KG_RELATIONS_KEY, []).extend([Relation(
                                                            source_id=str(source_id),
                                                            target_id=current_node.id_,
                                                            label=get_node_rel_string(NodeRelationship.CALLED_BY),
                                                            )])
    
    return list(nodes.values())


def main():
    # Neo4j connection setup
    graph_store = get_graph_store()
    # driver = get_driver()
    ## Delete all nodes and relationships
    # with driver.session() as session:
    #     session.execute_write(delete_all)
    
    # Paths from environment variables
    code_elements_path = 'data\output.json'
    with open(code_elements_path, 'r',encoding='utf-8') as file:
        data = json.load(file)  # Load the JSON data as a Python object (usually a list of dictionaries)
    nodes = create_node_and_relation(data)
    print(len(nodes))
    # os.environ['OPENAI_API_KEY']=os.getenv("OPENAI_API_KEY")
    # Load the existing PropertyGraphIndex
    # print(len(nodes))
    property_graph_index = PropertyGraphIndex(
        nodes=nodes,
        embed_model=embed_model,
        property_graph_store=graph_store,
        kg_extractors=[ImplicitPathExtractor()],
    )
    print(property_graph_index.property_graph_store)
    graph_store._driver.close()

if __name__ == "__main__":
    main()
