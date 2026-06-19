import requests
from langchain_core.prompts import ChatPromptTemplate,MessagesPlaceholder
from langchain_core.prompts.chat import SystemMessagePromptTemplate, HumanMessagePromptTemplate
from langchain_core.messages import BaseMessage
from langchain.chat_models.base import BaseChatModel
from langchain_openai import ChatOpenAI
from typing import List, Dict
from langfuse.decorators import observe, langfuse_context
from langfuse import Langfuse
import os
from pydantic import BaseModel, Field
from app.common.config import logger
from app.database.graphdb import GraphDatabaseConnection
graph_database_promtion = GraphDatabaseConnection()


class query_promotion(BaseModel):
    """
    Hàm `query_promotion` được chọn đối khi user cần truy vấn, tìm kiếm thông tin về chương trình khuyến mãi
    """
    query: str = Field(..., title="user_query", description="Câu truy vấn/câu hỏi của user về chương trình khuyến mãi")


@observe()
def retrieve_promotion(query: str) -> str:
    """
    This function is to retrivel the promotion information from the graph database
    Args:
        data (dict): The query to retrivel the promotion information
    Returns:
        Response: The response of the promotion information
    """

    langfuse = Langfuse()
    langfuse_prompt = langfuse.get_prompt("retrieve_promotion")
    prompt_config = langfuse_prompt.config
    model_name = prompt_config.get("model", os.getenv("LLM_GRAPH"))
    temperature = prompt_config.get("temperature", 0.0)
    mentions = prompt_config.get("mentions", {})
    if len(mentions) > 0:
        system_prompt = langfuse_prompt.compile(**mentions)
    else:
        system_prompt = langfuse_prompt.compile()

    
    retrival_engine = graph_database_promtion.get_retrieval_engine()

    prompt = ChatPromptTemplate.from_messages([
        SystemMessagePromptTemplate.from_template(system_prompt),
        HumanMessagePromptTemplate.from_template("{input}"),
    ])
    llm = ChatOpenAI(model=model_name, api_key=os.environ.get("OPENAI_API_KEY"), temperature=temperature)

    langfuse_handler = langfuse_context.get_current_langchain_handler()
    chain = prompt | llm
    query_graph = chain.invoke(
        {
            "input": query,
        }, config={"callbacks": [langfuse_handler]}
    )
    logger.debug(f"Query graph: {query_graph.content}")
    result = retrival_engine.retrieve(query_graph.content)
    response = "\n".join([node.text for node in result]) if result else "No promotion found"
    return response


@observe()
def query_promotion_func(**kwargs) -> str:
    
    """This function is to query the promotion information from the graph database
    
    Args:
        list_kargs (List[Dict]): List of dictionaries containing the key-words arguments for the function.
                                The function can be called multiple time with each dictionary in the list.
        user_query (str): The user's query
        chat_history (List[BaseMessage]): The chat history
        llm (BaseChatModel): The language model used to generate the response
    
    Returns:
        str: Your response
    
    """
    user_query = kwargs.get("user_query", None)
    chat_history = kwargs.get("chat_history", None)
    contextualized_query = kwargs.get("contextualized_query", None)

    if not user_query:
        raise ValueError("Missing the user_query argument. Please provide the user_query argument.")
    if not chat_history:
        raise ValueError("Missing the chat_history argument. Please provide the chat_history argument.")
    if not contextualized_query:
        raise ValueError("Missing the contextualized_query argument. Please provide the contextualized_query argument.")

    langfuse = Langfuse()
    # get prompt from function name
    langfuse_prompt = langfuse.get_prompt("query_promotion")
    prompt_config = langfuse_prompt.config
    model_name = prompt_config.get("model", os.getenv("LLM_ANSWER"))
    temperature = prompt_config.get("temperature", 0.0)
    mentions = prompt_config.get("mentions", {})

    response_retrieval = retrieve_promotion(contextualized_query)
    response_retrieval = response_retrieval.replace("{", "\n").replace("}", "\n")
    logger.debug(f"Response of retrieval: {response_retrieval}")

    if len(mentions) > 0:
        function_prompt = langfuse_prompt.compile(retrieved_doc=response_retrieval, **mentions)
    else:
        function_prompt = langfuse_prompt.compile(retrieved_doc=response_retrieval)

    prompt = ChatPromptTemplate.from_messages([
        SystemMessagePromptTemplate.from_template(function_prompt),
        MessagesPlaceholder(variable_name="chat_history"),
        HumanMessagePromptTemplate.from_template(user_query)
    ])
    
    llm = ChatOpenAI(model=model_name, api_key=os.getenv("OPENAI_API_KEY"), temperature=temperature)
    
    chain = prompt | llm
    langfuse_handler = langfuse_context.get_current_langchain_handler()
    response = chain.invoke({
        "chat_history": chat_history
    }, config={"callbacks": [langfuse_handler]})

    return response.content
