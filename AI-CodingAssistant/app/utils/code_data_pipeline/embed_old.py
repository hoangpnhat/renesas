
import json
from typing import Any, Dict, List, Optional, Sequence, Type, TYPE_CHECKING,Literal

from llama_index.core.data_structs import IndexLPG
from llama_index.core.base.base_retriever import BaseRetriever
from llama_index.core.llms import LLM
from llama_index.core.embeddings.utils import EmbedType, resolve_embed_model
from llama_index.core.callbacks import CallbackManager
from llama_index.core.graph_stores.simple_labelled import SimplePropertyGraphStore
from llama_index.core.graph_stores.types import (
    KG_NODES_KEY,
    KG_RELATIONS_KEY,
    VECTOR_SOURCE_KEY,
)
from llama_index.core.vector_stores.simple import DEFAULT_VECTOR_STORE
from llama_index.core.indices.base import BaseIndex
from llama_index.core.indices.property_graph.transformations import (
    ImplicitPathExtractor,
)
from llama_index.core.graph_stores.types import (
    LabelledNode,
    Relation,
    PropertyGraphStore,
    TRIPLET_SOURCE_KEY,
)
from llama_index.core.storage.docstore.types import RefDocInfo
from llama_index.core.storage.storage_context import StorageContext
from llama_index.core.vector_stores.types import BasePydanticVectorStore

if TYPE_CHECKING:
    from llama_index.core.indices.property_graph.sub_retrievers.base import (
        BasePGRetriever,
    )
from llama_index.core import PropertyGraphIndex,Settings
from llama_index.core.indices.property_graph import ImplicitPathExtractor
from llama_index.core.schema import TextNode, BaseNode, NodeRelationship,TransformComponent,MetadataMode
from llama_index.core.graph_stores.types import Relation, KG_NODES_KEY, KG_RELATIONS_KEY
from neo4j import GraphDatabase
from llama_index.graph_stores.neo4j import Neo4jPropertyGraphStore
from llama_index.embeddings.databricks import DatabricksEmbedding
from custom_neo4j_property_graph import CustomNeo4jPropertyGraphStore
import os
import sys
from dotenv import load_dotenv
if os.getcwd() not in sys.path: sys.path.append(os.getcwd())
# Load the .env file
load_dotenv()
# NEO4J_USERNAME=ad
# NEO4J_PASSWORD=Saigon123456
# NEO4J_URI=neo4j://104.41.147.37:7687
# CODE_ELEMENTS_PATH=AI-CodingAssistant\data\code_elements.json
# OPENAI_API_KEY=
# DATABRICKS_TOKEN=
# DATABRICKS_SERVING_ENDPOINT=https://adb-379144824042062.2.azuredatabricks.net/serving-endpoints
# MODEL_EMBEDDING_NAME=text-embedding-3-small


# Set up the DatabricksEmbedding class with the required model, API key and serving endpoint
os.environ["DATABRICKS_TOKEN"] = os.getenv('DATABRICKS_TOKEN', '')
os.environ["DATABRICKS_SERVING_ENDPOINT"] = 'https://adb-379144824042062.2.azuredatabricks.net/serving-endpoints'
embed_model = DatabricksEmbedding(model='ai-nonprod-coding-ais-text-embedding-3-small')


def delete_all(tx):
    tx.run("MATCH (n) DETACH DELETE n")

# def get_driver():
#     return GraphDatabase.driver('neo4j://52.183.98.147:7687', auth=('neo4j', 'Saigon123456'))
# def get_graph_store():
#     return Neo4jPropertyGraphStore(
#         username='neo4j',
#         password='Saigon123456',
#         url='neo4j://52.183.98.147:7687',
#     )
def get_driver():
    return GraphDatabase.driver(os.environ.get('NEO4J_URI'), auth=(os.environ.get('NEO4J_USERNAME'), os.environ.get('NEO4J_PASSWORD')))
def get_graph_store():
    return Neo4jPropertyGraphStore(
        username=os.environ.get('NEO4J_USERNAME'),
        password=os.environ.get('NEO4J_PASSWORD'),
        url=os.environ.get('NEO4J_URI'),
    )
def get_node_rel_string(relationship: NodeRelationship) -> str:
    return str(relationship).split(".")[-1]
def create_node_and_relation(funcs: List) -> Dict:
    nodes = []
    for index,func in enumerate(funcs):
        parent_node = TextNode(
            text=func['original_code'][:1000],
            id_=f"parent_{index}",
            metadata={
                "code_type": func['code_type'],
                "original_file": func['original_file']
            }
        )
        nodes.append(parent_node)
        child_nodes = []
        if func['chunks']:
            for i, chunk in enumerate(func['chunks']):
                child_node = TextNode(
                    text=chunk[:1000],
                    id_=f"child_{index}_{i}",
                    metadata={
                        "code_type": func['code_type'],
                        "original_file": func['original_file']
                    }
                )

                child_node.metadata[KG_RELATIONS_KEY]=[Relation(
                                                        source_id=parent_node.node_id,
                                                        target_id=child_node.node_id,
                                                        label=get_node_rel_string(NodeRelationship.PARENT),
                                                    )]
                nodes.append(child_node)
                child_nodes.append(child_node)

            for i in range(len(child_nodes) - 1):
                child_nodes[i].metadata[KG_RELATIONS_KEY].extend([Relation(
                                                            source_id=child_nodes[i].node_id,
                                                            target_id=child_nodes[i + 1].node_id,
                                                            label=get_node_rel_string(NodeRelationship.NEXT),
                                                            )])
    
    return nodes

def main():
    # Neo4j connection setup
    graph_store = get_graph_store()
    driver = get_driver()
    # Delete all nodes and relationships
    with driver.session() as session:
        session.execute_write(delete_all)

    
    # Paths from environment variables
    code_elements_path = 'data\code_elements.json'
    with open(code_elements_path, 'r',encoding='utf-8') as file:
        data = json.load(file)  # Load the JSON data as a Python object (usually a list of dictionaries)
    nodes = create_node_and_relation(data)
    
    # os.environ['OPENAI_API_KEY']=os.getenv("OPENAI_API_KEY")
    # Load the existing PropertyGraphIndex
    print(len(nodes))
    property_graph_index = PropertyGraphIndex(
        nodes=nodes,
        embed_model=embed_model,
        property_graph_store=graph_store,
        kg_extractors=[ImplicitPathExtractor()],
    )
    print(property_graph_index.property_graph_store)
    graph_store.close()

if __name__ == "__main__":
    main()
