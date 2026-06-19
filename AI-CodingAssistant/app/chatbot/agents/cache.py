from typing import List, Dict
from app.messages.base import CBaseMessage

class CCache:
    _instance = None

    def __new__(cls, *args, **kwargs):
        if cls._instance is None:
            cls._instance = super(CCache, cls).__new__(cls)
        return cls._instance

    def __init__(self):
        if not hasattr(self, 'initialized'):  # Prevent __init__ from running more than once
            self.initialized = True
            self.cache = {}  # Example cache storage

    def get(self, key):
        return self.cache.get(key, [])

    def set(self, key, value):
        self.cache[key] = self.get(key) + [value]

    def get_cache_from_message(self, message: CBaseMessage) -> List[Dict]:
        """
        This function gets the cache from the message object.

        Args:
            message (CBaseMessage): The message object
        
        Returns:
            List[Dict]: The cache from the message object
        """
        # If the current thread is empty, get the parent thread cache
        if len(self.get(message.thread_id)) == 0:
            parent_msg_cache = self.get(message.parent_thread_id + "_" + message.parent_message_id)
            # If the parent thread cache is not empty, set the current thread cache to the parent thread cache
            if message.thread_id and len(parent_msg_cache) > 0:
                self.set(message.thread_id, parent_msg_cache[-1])
            # Return the parent thread cache (list of dictionaries)
            return parent_msg_cache
        else:
            # Return the current thread cache (list of dictionaries)
            return self.get(message.thread_id)

    def set_cache_from_message(self, message: CBaseMessage, value):
        """
        This function sets the cache from the message object.

        Args:
            message (CBaseMessage): The message object
            value (Dict): The value to set
        
        Returns:
            None
        """
        # If the current thread is empty, set the parent thread cache
        if message.thread_id:
            self.set(message.thread_id, value)
        else:
            self.set(message.parent_thread_id + "_" + message.parent_message_id, value)