from typing import List
from datetime import datetime
from typing import List, Optional, Union
from langchain_core.messages import AIMessage, HumanMessage

from app.utils.image import convert_image_to_base64, download_image
from app.common.config import cfg


class CBaseMessage:
    def __init__(self, 
                 id: str,
                 text: str,
                 is_bot: bool, 
                 sent_at: Union[datetime, str, int],
                 sender_id: Optional[str],
                 receiver_id: Optional[str],
                 thread_id: Optional[str] = None,
                 message_id: Optional[str] = None, 
                 parent_message_id: Optional[str] = None, 
                 parent_thread_id: Optional[str] = None,
                 image_urls: List[str] = [],
                 message_type: Optional[str] = None, 
                 file_paths: List[str] = [],
                 sender_name: Optional[str] = None, 
                 receiver_name: Optional[str] = None):
        
        self.id = id
        self.thread_id = thread_id
        self.message_id = message_id
        self.parent_message_id = parent_message_id
        self.parent_thread_id = parent_thread_id
        self.text = text
        self.sender_id = sender_id
        self.sender_name = sender_name
        self.receiver_id = receiver_id
        self.receiver_name = receiver_name
        self.is_bot = is_bot
        self.image_urls = image_urls
        self.sent_at = sent_at
        self.message_type = message_type
        self.file_paths = file_paths

        if not self.thread_id and not (self.parent_thread_id and self.parent_message_id):
            raise ValueError("thread_id is required if both parent_thread_id AND parent_message_id are not provided")

    def to_langchain_message(self, apply_image: bool = False) -> AIMessage | HumanMessage:
        additional_kwargs={
                    "id": self.id,
                    "thread_id": self.thread_id,
                    "message_id": self.message_id,
                    "parent_message_id": self.parent_message_id,
                    "parent_thread_id": self.parent_thread_id,
                    "image_urls": self.image_urls,
                    "file_paths": self.file_paths,
                    "sender_id": self.sender_id,
                    "sender_name": self.sender_name,
                    "receiver_id": self.receiver_id,
                    "receiver_name": self.receiver_name,
                    "sent_at": self.sent_at,
                    "message_type": self.message_type,
                    "has_image": False
                }
        if self.is_bot:
            return AIMessage(
                content=self.text,
                additional_kwargs=additional_kwargs
            )
        else:
            if apply_image:
                content = [{"type": "text", "text": self.text}]
                # get the image in message
                for image_url in self.image_urls:
                    image, image_path = download_image(image_url)
                    img_base64 = convert_image_to_base64(image, 
                                                        quality=cfg.image_quality,
                                                        max_size=(cfg.image_with, cfg.image_height))
                    content.append({
                        "type": "image_url",
                        "image_url": {"url": f"data:image/jpeg;base64,{img_base64}"}
                    })
                additional_kwargs["has_image"] = True
                return HumanMessage(
                    content=content,
                    additional_kwargs=additional_kwargs
                )
            else:
                return HumanMessage(
                    content=self.text,
                    additional_kwargs=additional_kwargs
                )
	