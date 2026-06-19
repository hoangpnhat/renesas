from fastapi import FastAPI
from fastapi.responses import JSONResponse
from dotenv import load_dotenv, find_dotenv
import os
from pydantic import BaseModel
load_dotenv(find_dotenv(), override=True)

from app.gapo.webhook import gapo_app
from app.report.report import report_app
from app.chatbot.function_call.tool import CRUDTool, ToolSchema
from app.chatbot.query_router.route import CRUDRoute, RouteSchema

app = FastAPI()

app.mount("/gapo", gapo_app, name="gapo")

app.mount("/report", report_app, name="report")

# Define the routes
@app.get("/")
def health_check():
    return JSONResponse(content={"status": "ok"})

@app.get("/logs", tags=["logs"])
async def get_logs(n_lines: int = -1):
    try:
        if os.environ.get("ENV") == "dev":
            log_file_path = os.environ.get("DEV_LOG_FILE_PATH", "./logs/dev_data.log")
        else:
            log_file_path = os.environ.get("PRD_LOG_FILE_PATH", "./logs/data.log")
        with open(log_file_path, "r", encoding='utf-8') as f:
            logs = f.readlines()
            # logs = logs.split("\u001b[0m\n\u001b")
            if n_lines > 0:
                logs = logs[-n_lines:]
        # return logs
        return JSONResponse(content={"log_file": log_file_path,"logs": logs})
    except Exception as e:
        return JSONResponse(content={"error": str(e)})
    

class CTool(BaseModel):
    tool: ToolSchema
    route: RouteSchema

@app.post("/add_tool", tags=["tools"])
async def add_tool(data: CTool):
    try:
        crud_tool = CRUDTool()
        crud_route = CRUDRoute()

        tool = ToolSchema(**data.tool.model_dump())
        route = RouteSchema(**data.route.model_dump())
        
        if tool.name != route.name:
            raise ValueError(f"Tool name {tool.name} and route name {route.name} do not match. Please make sure they are the same.")
        
        if len(route.utterances) == 0:
            raise ValueError(f"Route {route.name} has no utterances. Please add utterances to the route.")
        
        tool_id = crud_tool.create_tool(tool)
        try:
            route_id = crud_route.create_route(route)
            if not route_id:
                raise ValueError(f"Failed to create route {route.name}. Please try again.")
        except Exception as e:
            crud_tool.collection.delete_one({"_id": tool_id})
            raise ValueError(f"Failed to create route {route.name}. Please try again.")
        return JSONResponse(content={"message": "Succesfully created a tool!", "tool": tool.name, "route": route.name})
    except Exception as e:
        return JSONResponse(content={"error": str(e)}, status_code=422)


@app.get("/tool", tags=["tools"])
async def get_tools(tool_name: str = None):
    try:
        crud_tool = CRUDTool()
        if tool_name:
            tool = crud_tool.get_tool(tool_name)
            return JSONResponse(content={"tool": tool.model_dump()})
        tools = crud_tool.get_all_tools()
        return JSONResponse(content={"tools": [tool.model_dump() for tool in tools]})
    except Exception as e:
        return JSONResponse(content={"error": str(e)}, status_code=422)
    

@app.post("/tool", tags=["tools"])
async def update_tool(update_data: ToolSchema):
    try:
        crud_tool = CRUDTool()
        tool_name = update_data.get("tool", {}).get("name", None)
        existed_tool = crud_tool.get_tool(tool_name)
        if not existed_tool:
            raise ValueError(f"Tool {tool_name} does not exist. Please create the tool instead.")
        tool = ToolSchema(**update_data.get("tool"))
        result = crud_tool.update_tool(tool_name, tool)
        return JSONResponse(content={"message": "Update tool successully!", "tool_name": tool.name, "id": result})
    except Exception as e:
        return JSONResponse(content={"error": str(e)}, status_code=422)
    

@app.post("/route", tags=["tools"])
async def update_route(update_data: RouteSchema):
    try:
        crud_route = CRUDRoute()
        route_name = update_data.get("route", {}).get("name", None)
        existed_route = crud_route.get_route(route_name)
        if not existed_route:
            raise ValueError(f"Route {route_name} does not exist. Please create the route instead.")
        route = RouteSchema(**update_data.get("route"))
        result = crud_route.update_route(route_name, route)
        return JSONResponse(content={"message": "Update route successully!", "route_name": route.name, "id": result})
    except Exception as e:
        return JSONResponse(content={"error": str(e)}, status_code=422)