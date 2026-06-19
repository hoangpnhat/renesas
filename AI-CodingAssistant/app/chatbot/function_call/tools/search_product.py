import requests
from langchain_core.prompts import ChatPromptTemplate,MessagesPlaceholder
from langchain_core.prompts.chat import SystemMessagePromptTemplate
from langchain_openai import ChatOpenAI
import json
from pydantic import BaseModel, Field
from langfuse import Langfuse
import os

from app.common.config import logger

class search_product(BaseModel):
    """
    Hàm `search_product` dùng để tìm kiếm thông tin sản phẩm dựa vào mã sản phẩm
    """
    product_code: str = Field(..., title="product_code", description="Mã sản phẩm cần tìm kiếm.")


def search_product_func(**kwargs) -> str:
    """
    This function is used to search for product information based on the product code and return the response to the user.

    Args:
        product_code (str): The product code
        chat_history (List[BaseMessage]): The chat history

    Returns:
        str: Your response
    
    """
    product_code = kwargs.get("product_code", None)
    chat_history = kwargs.get("chat_history", None)

    if not product_code:
        raise ValueError("Missing the product_code argument. Please provide the product_code argument.")
    if not chat_history:
        raise ValueError("Missing the chat_history argument. Please provide the chat_history argument.")

    list_of_product_code = []
    if isinstance(product_code, str):
        list_of_product_code.append(product_code.strip())
    elif isinstance(product_code, list):
        list_of_product_code = [code.strip() for code in product_code]

    langfuse = Langfuse()
    # get prompt from function name
    langfuse_prompt = langfuse.get_prompt("search_product")
    prompt_config = langfuse_prompt.config
    model_name = prompt_config.get("model", os.getenv("LLM_ANSWER"))
    temperature = prompt_config.get("temperature", 0.0)
    mentions = prompt_config.get("mentions", {})

    product_str = ""
    for product_code in list_of_product_code:
        url = os.getenv("PRODUCT_SEARCH_API") + f"?page=1&limit=10&search={product_code}"
        headers = {
            "Content-Type": "text/html"
        }
        response = requests.get(url, headers=headers)
        if response.status_code != 200:
            product_str += f"Không có thông tin về sản phẩm {product_code}. \n\n"
            logger.error(f"Error when searching product {product_code}. Status code: {response.status_code}, Response: {response.text}")
        
        xml_content = response.text
        json_content = json.loads(xml_content)

        for product in json_content['data']:
            product_info = f"""
            Mã sản phẩm: {product.get("code", "Không có mã sản phẩm")}
            Tên sản phẩm: {product.get("title", "Không có tên sản phẩm")}
            Tồn kho: {str(product.get("inventory_quantity", 0))}
            SKU: {product.get("sku", "Không có SKU")}
            Màu sắc: {product.get("color", "")}
            Size: {product.get("size", "")}
            """
            product_str += product_info + "\n\n\n"


    if len(mentions) > 0:
        function_prompt = langfuse_prompt.compile(product_info=product_str, **mentions)
    else:
        function_prompt = langfuse_prompt.compile(product_info=product_str)
    
    llm = ChatOpenAI(model=model_name, api_key=os.getenv("OPENAI_API_KEY"), temperature=temperature)

    prompt = ChatPromptTemplate.from_messages([
        SystemMessagePromptTemplate.from_template(function_prompt),
        MessagesPlaceholder(variable_name="chat_history"),

    ])
    
    chain = prompt | llm

    response = chain.invoke({
        "chat_history": chat_history
    })

    return response.content

