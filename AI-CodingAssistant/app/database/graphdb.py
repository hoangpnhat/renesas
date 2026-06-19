from llama_index.llms.openai import OpenAI
from dotenv import load_dotenv, find_dotenv
load_dotenv(find_dotenv(), override=True)

import sys
import os
if os.getcwd() not in sys.path: sys.path.append(os.getcwd())
from app.common.config import cfg, logger

from dotenv import load_dotenv, find_dotenv
import os
from llama_index.core import PropertyGraphIndex
from llama_index.embeddings.openai import OpenAIEmbedding
from llama_index.llms.openai import OpenAI
from app.database.neo4j_property_graph import CustomNeo4jPropertyGraphStore
from app.database.sub_retrivevers.llm_synonym import CustomLLMSynonymRetriever
from app.database.sub_retrivevers.vector import CustomVectorContextRetriever
load_dotenv(find_dotenv(), override=True)
os.environ.get("OPENAI_API_KEY")
#load the documents
from neo4j import GraphDatabase


class GraphDatabaseConnection:
    _instance = None

    def __new__(cls, *args, **kwargs):
        if not cls._instance:
            cls._instance = super(GraphDatabaseConnection, cls).__new__(cls)
        return cls._instance

    def __init__(self, url=None, username=None, password=None, 
                 model=os.environ.get("LLM_GRAPH"), temperature=0.2, api_key=None):
        """
        Initialize the GraphDatabaseConnection
        Args:
            url: The url of the NEO4j graph database
            username: The username of the NEO4j graph database
            password: The password of the NEO4j graph database
            model (str): The model to use for the query
            temperature (float): The temperature of the model
            api_key (str): The api key for the model
            """
        if not hasattr(self, '_initialized'):
            self._initialized = True
            self.model = model
            self.temperature = temperature
            self.api_key = api_key or os.environ.get("OPENAI_API_KEY")
            self.url = url or os.environ.get("NEO4J_URL")
            self.username = username or os.environ.get("NEO4J_USERNAME")
            self.password = password or os.environ.get("NEO4J_PASSWORD")

            self.drive = GraphDatabase.driver(self.url, auth=(self.username, self.password))

            self.graph_store = CustomNeo4jPropertyGraphStore(
                username=self.username,
                password=self.password,
                url=self.url,
            )
            self.index = PropertyGraphIndex.from_existing(
                property_graph_store=self.graph_store,
                llm=OpenAI(model=os.environ.get("LLM_GRAPH"), temperature=0.1),
                embed_model=OpenAIEmbedding(model_name=os.environ.get("GRAPH_EMBEDDING")),
                use_async=False
            )
           
            self.synonym_retriever = CustomLLMSynonymRetriever(
                self.index.property_graph_store,
                llm=OpenAI(model=os.environ.get("LLM_GRAPH"), temperature=0.2),
                # include source chunk text with retrieved paths
                include_text=False,
                max_keywords=10,
                # the depth of relations to follow after node retrieval
                path_depth=1,
            )
            self.vector_retriever = CustomVectorContextRetriever(
                self.index.property_graph_store,
                # only needed when the graph store doesn't support vector queries
                # vector_store=index.vector_store,
                embed_model=OpenAIEmbedding(model_name=os.environ.get("GRAPH_EMBEDDING")),
                # include source chunk text with retrieved paths
                include_text=False,
                # the number of nodes to fetch
                similarity_top_k=10,
                # the depth of relations to follow after node retrieval
                path_depth=1,
                # can provide any other kwargs for the VectorStoreQuery class
            )
            logger.info(f"GraphDatabaseConnection is initialized")
    def get_retrieval_engine(self):
        """
        Get the retrieval engine
        """
        return self.index.as_retriever(
            include_text=False,  # include source text, default True
            sub_retrievers=[self.synonym_retriever,self.vector_retriever]
        )

    def get_query_engine(self):
        """
        Get the query engine
        """
        return self.index.as_query_engine(
                include_text=False,
                llm= OpenAI(model=self.model, temperature=self.temperature, api_key=self.api_key),
                sub_retrievers=[self.synonym_retriever,self.vector_retriever]
            )
    def delete_all(self):
        """
        Delete all nodes and relationships in the graph database
        """
        with self.drive.session() as session:
            session.run("MATCH (n) DETACH DELETE n")
