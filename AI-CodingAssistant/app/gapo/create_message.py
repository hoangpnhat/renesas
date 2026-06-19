import requests
import os
import sys
from typing import List, Dict
if os.getcwd() not in sys.path: sys.path.append(os.getcwd())
from app.common.config import cfg, logger



class MessageSender:
    def __init__(self):
        self.url = os.environ.get("GAPO_BOT_API_URL")
        self.headers = {
            "x-gapo-api-key": os.environ.get("GAPO_BOT_KEY"),
            "Content-Type": "application/json"
        }

    def send_text_message_to_subthread(self,
                                       thread_id: int,
                                       bot_id: int,
                                       message_id: int,
                                       message: str,
                                       mention: dict = None) -> Dict | None:
        """
        This function sends a text message with mention to a subthread

        Args:
            thread_id (int): The thread id (parent thread id)
            bot_id (int): The bot id
            message_id (int): The message id (parent message id)
            message (str): The message to send
            mention (dict): The mention to send

        Returns:
            dict: The response from Gapo
        """
        data = {
            "thread_id": thread_id,
            "bot_id": bot_id,
            "body": {
                "type": "text",
                "text": message,
                "is_markdown_text": False,
                
            }
        }

        if message_id is not None:
            data["message_id"] = message_id

        if mention:
            data['body']['metadata']={}
            data['body']['metadata']['mentions']=[]
            for user in mention:
                data['body']['metadata']['mentions'].append(
                        {
                            "target": user['pic_gapo_id'], 
                            "length": len(user['pic_gapo_name']), 
                            "offset": len(data['body']['text'])
                            }
                    )
                data['body']['text']+= user['pic_gapo_name']

        try:
            response = requests.post(self.url, headers=self.headers, json=data)
            if response.status_code == 200:
                logger.debug("Message sent successfully")
                return response.json()['data']
            else:
                logger.error(f"Cannot send reply the message to gapo! \
                            Status code {response.status_code}, Response {response.json()} \
                            \n thread_id: {thread_id}, bot_id: {bot_id}, message_id: {message_id}, message: {message}")
                return None
        except Exception as e:
            logger.error(f"Cannot send reply the message to gapo! {e} \
                          \n thread_id: {thread_id}, bot_id: {bot_id}, message_id: {message_id}, message: {message}")
            return None
        
    
    def send_text_message_to_user(self, receiver_id: int, bot_id: int, message: str) -> Dict | None:
        """
        This function sends a text message to a user as a direct message

        Args:
            receiver_id (int): The receiver id
            bot_id (int): The bot id
            message (str): The message to send

        Returns:
            dict: The response from Gapo
        """
        data = {
            "receiver_id": receiver_id,
            "bot_id": bot_id,
            "body": {
                "type": "text",
                "text": message,
                "is_markdown_text": True
            }
        }
        try:
            response = requests.post(self.url, headers=self.headers, json=data)
            if response.status_code == 200:
                logger.debug("Message sent successfully")
                return response.json()['data']
            else:
                logger.error(f"Cannot send reply the message to gapo! \
                            Status code {response.status_code}, Response {response.json()} \
                            \n receiver_id: {receiver_id}, bot_id: {bot_id}, bot_id: {bot_id}, message: {message}")
                return None
        except Exception as e:
            logger.error(f"Cannot send reply the message to gapo! {e} \
                          \n receiver_id: {receiver_id}, bot_id: {bot_id}, bot_id: {bot_id}, message: {message}")
            return None
    
    def send_carousel_cards(self, thread_id: int, bot_id: int, message_id: int, carousel_cards: List[Dict]) -> Dict | None:
        """
        This function sends a carousel cards to a sub thread

        Args:
            thread_id (int): The thread id
            bot_id (int): The bot id
            carousel_cards (List[Dict]): List of carousel cards to send

        Returns:
            dict: The response from Gapo, if the message sent successfully. Otherwise, None
        """
        if len(carousel_cards) == 0 and len(carousel_cards) > 3:
            logger.error("The carousel cards must have at least 1 card and at most 3 cards!")
            raise ValueError("The carousel cards must have at least 1 card and at most 3 cards!")
        
        data = {
            "thread_id": int(thread_id),
            "bot_id": int(bot_id),
            "body": {
                "type": "carousel",
                "metadata": {
                    "carousel_cards": carousel_cards
                }
            }
        }

        response = requests.post(self.url, headers=self.headers, json=data)
        if response.status_code == 200:
            logger.debug("Message sent successfully")
            return response.json()['data']
        else:
            msg = f"Cannot send the carousel cards message to gapo! \
                        Status code {response.status_code}, Response {response.json()} \
                        \n thread_id: {thread_id}, bot_id: {bot_id}, carousel_cards: {carousel_cards}"
            logger.error(msg)
            raise requests.exceptions.RequestException(msg)