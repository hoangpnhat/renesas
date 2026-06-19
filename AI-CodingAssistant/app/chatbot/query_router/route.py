from pydantic import BaseModel
from typing import Optional, List, Dict
from datetime import datetime

from app.database.mongodb import MongoDBConnection

class RouteSchema(BaseModel):
    name: str
    utterances: List[str]
    created_at: datetime = datetime.now()
    user_id: Optional[str | None] = None


class CRUDRoute:
    def __init__(self):
        self.db = MongoDBConnection()
        self.collection = self.db.get_collection("routes")

    def create_route(self, route: RouteSchema) -> str:
        """Create a new route in the collection."""
        query = {"name": route.name}
        existed_route = self.collection.find_one(query)
        if existed_route:
            raise Exception(f"Route {route.name} already exists. Please update the route instead.")
        else:
            result = self.collection.insert_one(route.model_dump())
            return str(result.inserted_id) if result else None

    def get_route(self, route_name: str) -> RouteSchema:
        """Retrieve a route by its ID."""
        query = {"name": route_name}
        result = self.collection.find_one(query)
        route = RouteSchema(**result) if result else None
        return route

    def get_all_routes(self) -> List[RouteSchema]:
        """Retrieve all routes in the collection."""
        results = self.collection.find({})
        routes = [RouteSchema(**row) for row in results]
        return routes
    

    def update_route(self, route_name: str, update_data: Dict) -> bool:
        """Update a route by its route_name."""
        query = {"name": route_name}
        update = {"$set": update_data}
        result = self.collection.update_one(query, update)
        return str(result.upserted_id) if result else None

    def delete_route(self, route_name: str) -> bool:
        """Delete a route by its route_name."""
        query = {"name": route_name}
        result = self.collection.delete_one(query)
        return str(result.upserted_id) if result else None