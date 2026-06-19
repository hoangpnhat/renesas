import requests
import os
import sys
from typing import List, Dict
from app.common.config import cfg, logger
from app.gapo.gapo_token import tokenizer


class MessageGetter:
    def __init__(self):
        self.url = os.environ.get("GAPO_BASE_API_URL") + 'messages'
    
    def generate_headers(self):
        """
        Generate headers for the request

        Returns:
            Dict: The headers
        """
        try:
            access_token = tokenizer.get_access_token()
            logger.debug(f"Access token retrieved successfully: {access_token}")
        except Exception as e:
            logger.error(f"Failed to get access token from Gapo! {e}.")

        headers = {
            "Accept": "application/json",
            "X-Gapo-Workspace-Id": str(os.environ.get("GAPO_WORKSPACE_ID")),
            "Content-Type": "application/json",
            "Authorization": f"Bearer {access_token}"
        }
        return headers
        
    def get_messages(self, thread_id: int, page_size: int = cfg.n_subthread_mesages) -> List[Dict] | List:
        """
        Get messages from a subthread or direct message

        Args: 
            thread_id (int): The thread id
            page_size (int): The number of messages to retrieve
        
        Returns:
            List[Dict]: A list of messages
        """

        headers = self.generate_headers()

        params = {
            "thread_id": thread_id,
            "page_size": page_size
        }
        try:
            response = requests.get(self.url, headers=headers, params=params)
            if response.status_code == 200:
                logger.debug(f"Messages retrieved successfully. Content: {response.json()}")
                return response.json()['data']
            else:
                logger.error(f"Failed to retrieve messages from Gapo! \
                                Request url: {self.url}, \
                            Status code {response.status_code}, Response {response.json()}")
                return []
        except Exception as e:
            logger.error(f"Failed to retrieve messages from Gapo! {e}")
            return []
        
    def get_parent_message(self, parent_thread_id, parent_message_id):
        """
        Get the parent message of a subthread

        Args:
            parent_thread_id (int): The parent thread id
        
        Returns:
            Dict: The parent message
        """
        try:
            
            headers = self.generate_headers()
            url = os.environ.get("GAPO_BASE_API_URL") + 'messages'
            response = requests.get(url + f'/{parent_message_id}?thread_id={parent_thread_id}', headers=headers)
            if response.status_code == 200:
                logger.debug("Parent message retrieved successfully")
                return response.json()['data']
            else:
                logger.error(f"Failed to retrieve parent message from Gapo! \
                             Request url: {url}, \
                            Status code {response.status_code}, Response {response.json()}")
                return None
        except Exception as e:
            logger.error(f"Failed to retrieve parent message from Gapo! {e}")
            return None
