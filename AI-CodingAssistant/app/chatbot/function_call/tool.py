from pydantic import BaseModel
from typing import Optional, List, Dict, Callable
from datetime import datetime
import importlib.util
import pydantic
from pydantic import Field
from langfuse import Langfuse
from langchain_openai import ChatOpenAI
from langchain_core.prompts import ChatPromptTemplate,MessagesPlaceholder
from langchain_core.prompts.chat import SystemMessagePromptTemplate, HumanMessagePromptTemplate
from langfuse.decorators import observe, langfuse_context
import os
from app.database.mongodb import MongoDBConnection
from app.chatbot.query_router.router import Router

class ToolSchema(BaseModel):
    name: str
    issue_code: str
    type: str
    func_desc: str
    description: str
    is_active: bool = True
    created_at: datetime = datetime.now()
    prompt: Optional[str] = None


class CRUDTool:
    def __init__(self):
        self.db = MongoDBConnection()
        self.collection = self.db.get_collection("tools")

    def create_tool(self, tool: ToolSchema) -> str:
        """Create a new tool in the collection."""
        query = {"name": tool.name}
        existed_tool = self.collection.find_one(query)
        if existed_tool:
            raise Exception(f"Tool {tool.name} already exists. Please update the tool instead.")
        else:
            result = self.collection.insert_one(tool.model_dump())
            return str(result.inserted_id) if result else None

    def get_tool(self, tool_name: str) -> ToolSchema:
        """Retrieve a tool by its ID."""
        result = self.collection.find_one({'name': tool_name})
        tool = ToolSchema(**result) if result else None
        return tool

    def get_all_tools(self) -> List[ToolSchema]:
        """Retrieve all tools in the collection."""
        query = {"is_active": True}
        results = self.collection.find(query)
        tools = [ToolSchema(**row) for row in results]
        return tools
    

    def update_tool(self, tool_name: str, update_data: Dict) -> bool:
        """Update a tool by its tool_name."""
        query = {"name": tool_name}
        update = {"$set": update_data}
        result = self.collection.update_one(query, update)
        return str(result.upserted_id) if result else None

    def delete_tool(self, tool_name: str) -> bool:
        """Delete a tool by its tool_name."""
        query = {"name": tool_name}
        update = {"$set": {"is_active": False}}
        result = self.collection.update_one(query, update)
        return str(result.upserted_id) if result else None
    

class BaseTool(ToolSchema):
    function_schema: Optional[BaseModel] = None
    function: Optional[Callable] = None

    def load(self):
        """
        This function is to load the 'function_schema' and 'function'.
        """
        raise NotImplementedError("The 'load' method must be implemented in the subclass.")

    def execute(self):
        """
        This method is to execute the function.
        """
        raise NotImplementedError("The 'rexecuteun' method must be implemented in the subclass.")
    
    
class ExternalTool(BaseTool):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.load()

    def load(self):
        """
        This function is to load the 'function_schema' and 'function'.
        """
        package_name = "app.chatbot.function_call.tools." + self.name
        if importlib.util.find_spec(package_name):
            package = importlib.import_module(package_name)

        self.function_schema = package.__dict__.get(self.name)
        self.function = package.__dict__.get(self.name + "_func")

    def execute(self, **kwargs):
        """
        This method is to execute the function.
        """
        return self.function(**kwargs)
    

class RedirectTool(BaseTool):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.load()

    def load(self):
        """
        This function is to load the 'function_schema' and 'function'.
        """
        base_attributes = {
            '__doc__': self.func_desc,
            'query': (str, Field(description="The query from user"))
        }
        self.function_schema = pydantic.create_model(self.name, **base_attributes)
        
    @observe()
    def execute(self, **kwargs):
        """
        This method is to execute the function.
        """
        user_query = kwargs.get("user_query", None)
        chat_history = kwargs.get("chat_history", None)

        if not user_query:
            raise ValueError("Missing the user_query argument. Please provide the user_query argument.")
        if chat_history is None:
            raise ValueError("Missing the chat_history argument. Please provide the chat_history argument.")

        langfuse = Langfuse()
        # get prompt from function name
        langfuse_prompt = langfuse.get_prompt(self.name)
        prompt_config = langfuse_prompt.config
        model_name = prompt_config.get("model", os.getenv("LLM_ANSWER"))
        temperature = prompt_config.get("temperature", 0.0)
        mentions = prompt_config.get("mentions", {})
        if len(mentions) > 0:
            function_prompt = langfuse_prompt.compile(**mentions)
        else:
            function_prompt = langfuse_prompt.compile()
        if not function_prompt or len(function_prompt) == 0:
            function_prompt = "Hãy trả lời cho User như sau: 'Em xin lỗi, em chưa được học về vấn đề này. Nhờ anh/chị liên hệ bộ phận CNTT anh <@CNTT. Phạm Hoài Nguyên (id:1029514815)> để được hỗ trợ ạ.'"

        langfuse_tail_prompt = langfuse.get_prompt("redirect_tail_prompt")
        tail_prompt_config = langfuse_tail_prompt.config
        tail_mentions = tail_prompt_config.get("mentions", {})
        if len(tail_mentions) > 0:
            tail_prompt = langfuse_tail_prompt.compile(**tail_mentions)
        else:
            tail_prompt = langfuse_tail_prompt.compile()
        if not tail_prompt:
            tail_prompt = ""
        
        function_prompt += tail_prompt 
        llm = ChatOpenAI(model=model_name, api_key=os.getenv("OPENAI_API_KEY"), temperature=temperature)
        prompt_template = ChatPromptTemplate.from_messages([
                SystemMessagePromptTemplate.from_template(function_prompt),
                MessagesPlaceholder(variable_name="chat_history"),
                HumanMessagePromptTemplate.from_template("{input}"),
            ])
        chain = prompt_template | llm
        langfuse_handler = langfuse_context.get_current_langchain_handler()
        result = chain.invoke(
            {
                "input": user_query,
                "chat_history": chat_history
            }, config={"callbacks": [langfuse_handler]})
        return result.content
    

class ToolManager:
    def __init__(self):
        self.tools = {}
        self.load_tools()

    def load_tools(self):
        """
        This function is to load all tools.
        """
        crud_tool = CRUDTool()
        tools = crud_tool.get_all_tools()
        for tool in tools:
            if tool.type == "external":
                self.tools[tool.name] = ExternalTool(**tool.model_dump())
            elif tool.type == "redirect":
                self.tools[tool.name] = RedirectTool(**tool.model_dump())

    def get_tool(self, tool_name: str) -> BaseTool:
        """
        This function is to get the tool by its tool_name.
        """
        return self.tools.get(tool_name, None)

    def execute_tool(self, tool_name: str, **kwargs):
        """
        This function is to execute the tool by its tool_name.
        """
        tool = self.get_tool(tool_name)
        if tool:
            return tool.execute(**kwargs)
        else:
            raise Exception(f"Tool {tool_name} does not exist.")
    
    def get_all_tools(self) -> Dict[str, BaseTool]:
        """
        This function is to get all tools.
        """
        return self.tools
    
    def route_query(self, query: str, top_k: int) -> List[str]:
        """
        This function is to route the query to the appropriate tools.

        Args:
            query (str): The query to route
            top_k (int): The number of tools to return
        """
        router = Router(top_k=top_k)
        return [route.name for route in router.route(query)]