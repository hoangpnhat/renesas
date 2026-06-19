import logging
import os
import sys
from pydantic import BaseModel, HttpUrl
from typing import Optional, Dict, Any, List
import pymongo

if os.getcwd() not in sys.path: sys.path.append(os.getcwd())
from app.database.mongodb import MongoDBConnection
from app.database.base import BaseCollection
from app.database.schemas import SubThread

class ThreadCollection(BaseCollection):
    def __init__(self):
        super().__init__("threads")

class SubThreadCollection(BaseCollection):
    def __init__(self):
        super().__init__("subthreads")

    def find_by_thread_id(self, thread_id: int,parent_message_id:int) -> List[Dict]:
        """Find all subthread documents by thread_id."""
        results = self.collection.find({"thread_id": thread_id,"parent_message_id":parent_message_id}).sort("message_id", pymongo.ASCENDING)
        return [{**row, "_id": str(row["_id"])} for row in results]

    
