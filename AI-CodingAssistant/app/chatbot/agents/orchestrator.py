import os
import sys
from langchain_core.messages import BaseMessage
from datetime import datetime
from langchain_openai import ChatOpenAI
import langfuse
from langfuse import Langfuse
from langfuse.decorators import observe, langfuse_context
from typing import List, Dict, Tuple

from app.common.config import logger, cfg
from app.messages.base import CBaseMessage
from app.chatbot.agents.utils import contextualize_query, pick_tool, check_user_satisfaction
from app.chatbot.agents.cache import CCache

from app.utils.str import extract_and_remove_dict_from_string
from app.chatbot.function_call.tool import ToolManager


OPENAI_API_KEY = os.environ.get("OPENAI_API_KEY", "")

llm_answer = ChatOpenAI(model=os.environ.get("LLM_ANSWER"), api_key=OPENAI_API_KEY, temperature=0.0)
llm_observation = ChatOpenAI(model=os.environ.get("LLM_OBSERVATION"), api_key=OPENAI_API_KEY, temperature=0.0)
llm_contextualization = ChatOpenAI(model=os.environ.get("LLM_CONTEXTUALIZE"), api_key=OPENAI_API_KEY, temperature=0.0)
llm_select_tool = ChatOpenAI(model=os.environ.get("LLM_SELECT_TOOL"), api_key=OPENAI_API_KEY, temperature=0.0)


def generate_answer(user_message: CBaseMessage, 
                    chat_history: List[BaseMessage], 
                    chat_history_wt_image: List[BaseMessage]) -> Tuple[str, Dict, str, str]:
    """
    This function takes a user message and chat history and returns an answer.

    Args:
        user_message (CBaseMessage): The user message to answer
        chat_history (List[CBaseMessage]): The chat history to use for answering

    Returns:
        str: The answer
    """
    cache = CCache()
    if len(cache.cache) > 200:
        cache.cache = {}
    # Get the list of cache from the user message(every message will have a cache, so a thread can have multiple caches)
    cached_response = cache.get_cache_from_message(user_message)
    lastest_cache = cached_response[-1] if len(cached_response) > 0 else {}
    is_user_satisfied = check_user_satisfaction(user_message.text, chat_history_wt_image)
    last_used_tool = lastest_cache.get('selected_tool', None)
    last_used_tool_name = last_used_tool.get('name', "unclear_issue") if last_used_tool else "unclear_issue"

    contextualized_query = user_message.text
    tool_manager = ToolManager()
    tool_names = []
    # If the user is satisfied with the last response and the last function is not unclear_issue or chitchat, use cache
    if last_used_tool_name not in ('unclear_issue', "chitchat") and is_user_satisfied:
        # get the last selected tool from cache
        tool_names = [last_used_tool_name]
        # If the last tool is query_promotion, alway contextualize the query
        if "query_promotion" in tool_names:
            contextualized_query = contextualize_query(user_message.text, chat_history_wt_image)
    else:
        contextualized_query = contextualize_query(user_message.text, chat_history_wt_image)
        # Pick best tools for the user using senmantic route
        tool_names = tool_manager.route_query(contextualized_query, cfg.top_k)
        logger.debug(f"Re-route tools: {tool_names}")
    
    # if the last tool is not in the list of tools, add it to the list
    if last_used_tool_name not in(tool_names) and last_used_tool_name not in ('unclear_issue', "chitchat"):
        tool_names = [last_used_tool_name] + tool_names

    logger.debug(f"Last used tool: {last_used_tool_name}")
    logger.debug(f"Tool names: {tool_names}")
    logger.debug(f"Contextualized query: {contextualized_query}")

    # get all tool schemas from 
    appropriate_tool_schemas = [tool_manager.get_tool(tool_name).function_schema for tool_name in tool_names]

    # Tools selected by llm
    contextualized_query = contextualized_query if contextualized_query else user_message.text

    picked_tools = pick_tool(user_message.text, contextualized_query, chat_history_wt_image, appropriate_tool_schemas)
    logger.debug(f"Picked tools: {picked_tools}")

    if len(picked_tools) == 0:
        picked_tools = [{}]
    # Only get the first tool
    selected_tool_name = picked_tools[0].get('name', None)
    params = picked_tools[0].get('args', {})
    if not selected_tool_name:
        selected_tool_name = "unclear_issue"
        params = {'query': user_message.text}

    # Execute the selected tool
    answer = tool_manager.execute_tool(tool_name=selected_tool_name, 
                                       user_query=user_message.text, 
                                       chat_history=chat_history, 
                                       **params)
    
    # Save the selected tool to cache
    cache.set_cache_from_message(user_message, {'selected_tool': {'name': selected_tool_name, 'args': params},
                                "timestamp": datetime.now().isoformat(),
                                'contextualized_query': contextualized_query})

    return answer, tool_manager.get_tool(selected_tool_name).issue_code, selected_tool_name
