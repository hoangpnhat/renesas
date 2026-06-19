import sys
import os
from pymongo import MongoClient, errors
import logging

if os.getcwd() not in sys.path: sys.path.append(os.getcwd())
from app.common.config import cfg, logger

class MongoDBConnection:
    _instance = None

    def __new__(cls, *args, **kwargs):
        if not cls._instance:
            cls._instance = super(MongoDBConnection, cls).__new__(cls)
        return cls._instance

    def __init__(self, uri=os.environ.get("MONGODB_URI"), db_name=os.environ.get("MONGODB_NAME")):
        if not hasattr(self, '_initialized'):
            self._initialized = True
            try:
                self._client = MongoClient(uri)
                self._db = self._client[db_name]
                logger.debug(f"Connected to MongoDB database: {db_name}")
            except errors.ConnectionError as e:
                logger.error(f"Could not connect to MongoDB: {e}")

    def get_database(self):
        return self._db

    def get_collection(self, collection_name):
        return self._db[collection_name]
    
