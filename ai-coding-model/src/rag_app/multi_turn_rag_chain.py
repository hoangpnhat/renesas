from operator import itemgetter
import mlflow
import os

from databricks.vector_search.client import VectorSearchClient

from langchain_community.chat_models import ChatDatabricks
from langchain_community.vectorstores import DatabricksVectorSearch

from langchain_core.runnables import RunnableLambda
from langchain_core.output_parsers import StrOutputParser
from langchain_core.prompts import (
    PromptTemplate,
    ChatPromptTemplate,
    MessagesPlaceholder,
)
from langchain_core.runnables import (
    RunnablePassthrough,
    RunnableBranch,
    RunnableParallel,
)
from langchain_core.messages import HumanMessage, AIMessage

# import sys
# sys.path.append(os.path.join(os.getcwd(), "utils"))

from llama_index.embeddings.databricks import DatabricksEmbedding
from llama_index.core import PropertyGraphIndex
from llama_index.core.schema import QueryBundle
from llama_index.llms.databricks import Databricks

from utils.parent_retriver import ParentVectorContextRetriever
from utils.custom_neo4j_graph import CustomNeo4jPropertyGraphStore

## Enable MLflow Tracing
mlflow.langchain.autolog()


############
# Helper functions
############
# Return the string contents of the most recent message from the user
def extract_user_query_string(chat_messages_array):
    return chat_messages_array[-1]["content"]


# Return the chat history, which is is everything before the last question
def extract_chat_history(chat_messages_array):
    return chat_messages_array[:-1]


# Load the chain's configuration
model_config = mlflow.models.ModelConfig(development_config="rag_chain_config.yaml")

databricks_resources = model_config.get("databricks_resources")
retriever_config = model_config.get("retriever_config")
llm_config = model_config.get("llm_config")

############
# Connect to the Vector Search Index
############
vs_client = VectorSearchClient(disable_notice=True)
vs_index = vs_client.get_index(
    endpoint_name=databricks_resources.get("vector_search_endpoint_name"),
    index_name=retriever_config.get("vector_search_index"),
)
vector_search_schema = retriever_config.get("schema")


# post-processing for retrieved documents
def get_list_page_content(result):
    result_string = result.get("result")
    docs = result.get("context_to_output")

    document_uri_column = vector_search_schema.get("document_uri")

    docs_to_show = []
    for index, doc in enumerate(docs):
        if f"[source111{index}]" in result_string:

            if len(docs_to_show) == 0:
                result_string = result_string.replace(
                    f"[source111{index}]",
                    f"please check {os.path.basename(doc.metadata[document_uri_column])}",
                )
            else:
                result_string = result_string.replace(f"[source111{index}]", " ")
            docs_to_show.append(doc)

    docs = docs_to_show

    return {
        "reference": [
            {"content": d.page_content, "doc_uri": d.metadata[document_uri_column]}
            for d in docs
        ],
        "result": result_string,
    }


############
# Turn the Vector Search index into a LangChain retriever
############
vector_search_as_retriever = DatabricksVectorSearch(
    vs_index,
    text_column=vector_search_schema.get("chunk_text"),
    columns=[
        vector_search_schema.get("primary_key"),
        vector_search_schema.get("chunk_text"),
        vector_search_schema.get("document_uri"),
    ],
).as_retriever(search_kwargs=retriever_config.get("parameters"))

############
# Required to:
# 1. Enable the RAG Studio Review App to properly display retrieved chunks
# 2. Enable evaluation suite to measure the retriever
############

mlflow.models.set_retriever_schema(
    primary_key=vector_search_schema.get("primary_key"),
    text_column=vector_search_schema.get("chunk_text"),
    doc_uri=vector_search_schema.get(
        "document_uri"
    ),  # Review App uses `doc_uri` to display chunks from the same document in a single view
)


class GraphDatabaseRetriever:
    def __init__(self):
        # Initialize the embedding model
        embed_model = DatabricksEmbedding(
            model=databricks_resources.get("graph_embedding_name")
        )

        llm = Databricks(
            model=databricks_resources.get("graph_llm_name"),
            api_key=os.environ["DATABRICKS_TOKEN"],
            api_base=os.environ["DATABRICKS_SERVING_ENDPOINT"],
        )

        username = os.getenv("NEO4J_USERNAME")
        password = os.getenv("NEO4J_PASSWORD")
        url = os.getenv("NEO4J_URI")
        dim = 1536
        graph_store = CustomNeo4jPropertyGraphStore(
            username=username, password=password, url=url
        )

        property_graph_index = PropertyGraphIndex.from_existing(
            property_graph_store=graph_store, embed_model=embed_model, llm=llm
        )
        graph_store.close()

        # # # Assuming you have an embedding model set up
        self.vector_retriever = ParentVectorContextRetriever(
            property_graph_index.property_graph_store,
            include_text=True,  # Include source chunk text with retrieved paths
            embed_model=embed_model,
        )

    def retrieve(self, query):
        query = QueryBundle(query_str=query)
        retriever = self.vector_retriever.retrieve_from_graph(query)
        return retriever


def graph_call(querry):
    # Vector_search context
    result = graph_db_retriever.retrieve(querry)
    return result


############
# Method to format the docs returned by the retriever into the prompt
############
def format_context(contexts):
    # Vector_search context
    vector_search_context = contexts.get("vector_db_context")
    chunk_template = retriever_config.get("chunk_template")
    chunk_contents = [
        chunk_template.format(
            chunk_text=f"[source111{index}] : " + d.page_content,
            document_uri=d.metadata[vector_search_schema.get("document_uri")],
        )
        for index, d in enumerate(vector_search_context)
    ]

    # Graph_search context
    graph_context = contexts.get("graph_db_context")
    # Filter some contexts which have score higher than 0.8
    graph_context = [context for context in graph_context if context[1] >= 0.74]
    chunk_template_2 = '"Reference Code": {chunk_text} \n'
    chunk_contents_2 = [
        chunk_template_2.format(
            chunk_text=d[0]["original_file"] + " file :" + d[0]["content"],
            document_uri=d[0]["original_file"],
        )
        for d in graph_context
    ]

    chunk_contents = chunk_contents + chunk_contents_2

    return "\n".join(chunk_contents)


############
# Graph retriever
############
graph_db_retriever = GraphDatabaseRetriever()

############
# Prompt Template for generation
############


def choose_prompt(retrieval_place):
    """This function to choose prompt which will be used to make system prompt for the querry

    Args:
        retrieval_place (String): string of retrieval place, may include ["document", "code_base"]
    Returns:
        String : prompt
    """

    if "code_base" in retrieval_place and "document" in retrieval_place:
        return llm_config.get("llm_system_prompt_for_document_and_code")
    if "code_base" in retrieval_place and "document" not in retrieval_place:
        return llm_config.get("llm_system_prompt_for_code")
    if "document" in retrieval_place and "code_base" not in retrieval_place:
        return llm_config.get("llm_system_prompt_for_document")
    return llm_config.get("llm_system_prompt_default")


prompt = ChatPromptTemplate.from_messages(
    [
        (  # System prompt contains the instructions
            "system",
            "{system_prompt}. \n Contexts: \n{context}",
        ),
        # If there is history, provide it.
        # Note: This chain does not compress the history, so very long converastions can overflow the context window.
        MessagesPlaceholder(variable_name="formatted_chat_history"),
        # User's most current question
        ("user", "{question}"),
    ]
)


# Format the converastion history to fit into the prompt template above.
def format_chat_history_for_prompt(chat_messages_array):
    history = extract_chat_history(chat_messages_array)
    formatted_chat_history = []
    if len(history) > 0:
        for chat_message in history:
            if chat_message["role"] == "user":
                formatted_chat_history.append(
                    HumanMessage(content=chat_message["content"])
                )
            elif chat_message["role"] == "assistant":
                formatted_chat_history.append(
                    AIMessage(content=chat_message["content"])
                )
    return formatted_chat_history


############
# Prompt Template for query rewriting to allow converastion history to work - this will translate a query such as "how does it work?" after a question such as "what is spark?" to "how does spark work?".
############
query_rewrite_template = """Based on the chat history below, we want you to generate a query for an external data source to retrieve relevant documents so that we can better answer the question. The query should be in natural language. The external data source uses similarity search to search for relevant documents in a vector space. So the query should be similar to the relevant documents semantically. Answer with only the query. Do not add explanation.

Chat history: {chat_history}

Question: {question}"""

query_rewrite_prompt = PromptTemplate(
    template=query_rewrite_template,
    input_variables=["chat_history", "question"],
)


############
# FM for generation
############
model = ChatDatabricks(
    endpoint=databricks_resources.get("llm_endpoint_name"),
    extra_params=llm_config.get("llm_parameters"),
)

############
# RAG Chain
############

chain = (
    {
        "question": itemgetter("messages") | RunnableLambda(extract_user_query_string),
        "chat_history": itemgetter("messages") | RunnableLambda(extract_chat_history),
        "formatted_chat_history": itemgetter("messages")
        | RunnableLambda(format_chat_history_for_prompt),
        "retrieval_place": itemgetter("retrieval_place"),
    }
    | RunnablePassthrough()
    | {
        "context": RunnableBranch(  # Only re-write the question if there is a chat history
            (
                lambda x: len(x["chat_history"]) > 0,
                query_rewrite_prompt | model | StrOutputParser(),
            ),
            itemgetter("question"),
        ),
        "formatted_chat_history": itemgetter("formatted_chat_history"),
        "question": itemgetter("question"),
        "retrieval_place": itemgetter("retrieval_place"),
    }
    | RunnablePassthrough()
    | {
        "context": {
            "vector_db_context": RunnableLambda(
                lambda x: (
                    itemgetter("context") | vector_search_as_retriever
                    if "document" in x["retrieval_place"]
                    else []
                )
            ),
            "graph_db_context": RunnableBranch(
                (
                    lambda x: "code_base" in x["retrieval_place"],
                    lambda x: graph_call(x["context"]),
                ),
                lambda x: [],
            ),
        }
        | RunnableLambda(format_context),
        "context_to_output": RunnableLambda(
            lambda x: (
                itemgetter("context") | vector_search_as_retriever
                if "document" in x["retrieval_place"]
                else []
            )
        ),
        "formatted_chat_history": itemgetter("formatted_chat_history"),
        "question": itemgetter("question"),
        "retrieval_place": itemgetter("retrieval_place"),
        "system_prompt": itemgetter("retrieval_place") | RunnableLambda(choose_prompt),
    }
    | {
        "prompt": prompt,
        "retrieval_place": itemgetter("retrieval_place"),
        "context_to_output": itemgetter("context_to_output"),
    }
    | {
        "result": itemgetter("prompt") | model,
        "retrieval_place": itemgetter("retrieval_place"),
        "context_to_output": itemgetter("context_to_output"),
    }
    | {
        "result": itemgetter("result") | StrOutputParser(),
        "context_to_output": itemgetter("context_to_output"),
    }
    | RunnableLambda(get_list_page_content)
    | RunnableParallel(
        retrieved_docs=itemgetter("reference"),
        response=itemgetter("result") | StrOutputParser(),
    )
)

## Tell MLflow logging where to find your chain.
# `mlflow.models.set_model(model=...)` function specifies the LangChain chain to use for evaluation and deployment.  This is required to log this chain to MLflow with `mlflow.langchain.log_model(...)`.

mlflow.models.set_model(model=chain)
