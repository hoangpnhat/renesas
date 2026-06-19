import os
from langchain_core.prompts import ChatPromptTemplate,MessagesPlaceholder
from langchain_core.prompts.chat import SystemMessagePromptTemplate, HumanMessagePromptTemplate
from langchain_core.messages import BaseMessage
from langchain_core.output_parsers.string import StrOutputParser
from typing import List, Callable, Dict
from typing import List, Dict
from langfuse.decorators import observe, langfuse_context
from langfuse import Langfuse
from langchain_openai import ChatOpenAI


@observe()
def contextualize_query(user_query: str, chat_history: List[BaseMessage]) -> str:
    """
    This function takes a user query and a chat history and returns a contextualized query.

    Args:
        user_query (str): The user query to contextualize
        chat_history (List[BaseMessage]): The chat history to use for contextualization

    Returns:
        str: The contextualized query
    """
    langfuse = Langfuse()
    langfuse_prompt = langfuse.get_prompt("contextualize_query")
    prompt_config = langfuse_prompt.config
    model_name = prompt_config.get("model", os.getenv("LLM_SELECT_TOOL"))
    temperature = prompt_config.get("temperature", 0.0)
    mentions = prompt_config.get("mentions", {})
    if len(mentions) > 0:
        system_prompt = langfuse_prompt.compile(**mentions)
    else:
        system_prompt = langfuse_prompt.compile()

    contextualize_q_prompt = ChatPromptTemplate.from_messages(
        [
            ("system", system_prompt),
            MessagesPlaceholder("chat_history"),
            ("human", "{input}"),
        ]
    )
    llm = ChatOpenAI(model=model_name, api_key=os.getenv("OPENAI_API_KEY"), temperature=temperature)
    chain = contextualize_q_prompt | llm
    langfuse_handler = langfuse_context.get_current_langchain_handler()
    ai_response = chain.invoke(
        {"chat_history": chat_history, "input": user_query},
        config={"callbacks": [langfuse_handler]}
    )
    return ai_response.content


@observe()
def pick_tool(user_query: str,
              contextualized_query: str,
              chat_history: List[BaseMessage],
              tools: List[Callable]) -> List[Dict]:
    """
    This function takes a user query, a system prompt, a chat history, and a list of tools and returns the tool calls.

    Args:
        user_query (str): The user query to answer
        contextualized_query (str): The contextualized query to use for answering
        chat_history (List[BaseMessage]): The chat history to use for answering
        tools (List[Callable]): The list of tools to use for answering

    Returns:
        List[Dict]: The tool calls for the given user query. 
                    For example: [{'name': 'multiply', 'args': {'a': 119, 'b': 8}, 'id': 'call_RofMKNQ2qbWAFaMs'}]
    """
    langfuse = Langfuse()
    langfuse_prompt = langfuse.get_prompt("pick_tool")
    prompt_config = langfuse_prompt.config
    model_name = prompt_config.get("model", os.getenv("LLM_SELECT_TOOL"))
    temperature = prompt_config.get("temperature", 0.0)
    mentions = prompt_config.get("mentions", {})
    if len(mentions) > 0:
        pick_tool_prompt = langfuse_prompt.compile(**mentions)
    else:
        pick_tool_prompt = langfuse_prompt.compile()
    
    prompt_template = ChatPromptTemplate.from_messages([
            SystemMessagePromptTemplate.from_template(pick_tool_prompt),
            MessagesPlaceholder(variable_name="chat_history"),
            HumanMessagePromptTemplate.from_template("{input}"),
            MessagesPlaceholder(variable_name="agent_scratchpad")
        ])
    llm = ChatOpenAI(model=model_name, api_key=os.getenv("OPENAI_API_KEY"), temperature=temperature)
    chain = prompt_template | llm.bind_tools(tools)
    langfuse_handler = langfuse_context.get_current_langchain_handler()
    response = chain.invoke(
                {
                    "input": user_query, 
                    "chat_history": chat_history,
                    "agent_scratchpad": []
                }, 
                config={"callbacks": [langfuse_handler]})
    return response.tool_calls
    

@observe()
def check_user_satisfaction(user_query: str, chat_history: List[BaseMessage]) -> bool:
    """
    This function to use llm model to check user satisfaction

    Args:
        user_query (str): The user query to answer
        chat_history (List[BaseMessage]): The chat history to use for answering

    Returns:
        bool: The user satisfaction. True if user is satisfied, False otherwise
    """

    langfuse = Langfuse()
    langfuse_prompt = langfuse.get_prompt("check_user_satisfaction")
    prompt_config = langfuse_prompt.config
    model_name = prompt_config.get("model", os.getenv("LLM_SELECT_TOOL"))
    temperature = prompt_config.get("temperature", 0.0)
    mentions = prompt_config.get("mentions", {})
    if len(mentions) > 0:
        system_prompt = langfuse_prompt.compile(**mentions)
    else:
        system_prompt = langfuse_prompt.compile()

    satisfaction_prompt = ChatPromptTemplate.from_messages(
        [
            ("system", system_prompt),
            MessagesPlaceholder("chat_history"),
            ("human", "{input}")
        ]
    )

    llm = ChatOpenAI(model=model_name, api_key=os.getenv("OPENAI_API_KEY"), temperature=temperature)
    chain = satisfaction_prompt | llm | StrOutputParser()
    langfuse_handler = langfuse_context.get_current_langchain_handler()
    ai_response = chain.invoke(
        {"chat_history": chat_history, "input": user_query},
        config={"callbacks": [langfuse_handler]}
    )

    if ai_response.upper() == "NO" or ai_response.upper() == "KHÔNG" or "NO" in ai_response.upper():
        return False
    return True
