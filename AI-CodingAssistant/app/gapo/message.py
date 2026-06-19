from pydantic import BaseModel
from typing import List
from datetime import datetime
import time
from typing import TypeVar, List, Dict
from app.messages.base import CBaseMessage
from typing import Optional, Union
from app.utils.image import is_image


class WebhookMessage(CBaseMessage):
    def __init__(self, gapo_msg: Dict):
        message = gapo_msg.get('message')
        metadata = message.get('metadata', {})
        thread = message.get('thread', {})
        user = message.get('user', {})

        self.id = str(gapo_msg.get("id"))
        self.text = message.get('text')
        self.sent_at = int(time.time())

        self.sender_id = str(user.get('id'))
        self.sender_name = user.get('name')
        self.receiver_id = gapo_msg.get('to_bot_id')
        self.receiver_name = None

        self.message_type = message.get('type')
        self.thread_type = thread.get('type')
        self.is_markdown_text = metadata.get('is_markdown_text', False)

        media_files = metadata.get("media", [])
        self.image_urls = [
            file_path for file_path in media_files if is_image(file_path)]

        self.file_paths = [
            file_path for file_path in media_files if not is_image(file_path)]

        self.mention = metadata.get("mentions", [])
        self.event_type = gapo_msg.get('event')


class ParentThreadMessage(WebhookMessage):
    def __init__(self, gapo_msg: Dict):
        super().__init__(gapo_msg)
        self.is_bot = False  # parent thread message always comes from a human
        if self.thread_type == 'group':
            self.thread_id = None
            self.message_id = None
            self.parent_thread_id = str(gapo_msg.get('thread_id'))
            self.parent_message_id = str(gapo_msg.get('message', {}).get("id"))


class SubThreadMessage(WebhookMessage):
    def __init__(self, gapo_msg: Dict):
        super().__init__(gapo_msg)
        self.is_bot = False  # Only human can trigger the webhook by mentioning the bot
        if self.thread_type == 'subthread':
            self.thread_id = str(gapo_msg.get("thread_id"))
            self.message_id = str(gapo_msg.get("message", {}).get("id"))
            self.parent_thread_id = str(gapo_msg.get(
                "message", {}).get('thread', {}).get('parent_id'))
            self.parent_message_id = str(gapo_msg.get(
                "message", {}).get('thread', {}).get('root_message_id'))


class DirectMessage(WebhookMessage):
    def __init__(self, gapo_msg: Dict):
        super().__init__(gapo_msg)
        self.is_bot = False  # Only human can trigger the webhook by mentioning the bot
        if self.thread_type == 'direct':
            self.thread_id = str(gapo_msg.get("thread_id"))
            self.message_id = str(gapo_msg.get("message", {}).get("id"))
            self.parent_thread_id = str(gapo_msg.get(
                "message", {}).get('thread', {}).get('parent_id'))
            self.parent_message_id = str(gapo_msg.get(
                "message", {}).get('thread', {}).get('root_message_id'))


class APIMessage(CBaseMessage):
    def __init__(self, data: Dict, bot_id: str = None) -> None:
        body = data.get('body', {})
        sender = data.get('sender', {})
        metadata = body.get('metadata', {}) or {}

        self.id = str(data.get('id'))
        self.text = body.get('text')
        self.sent_at = data.get('created_at')
        self.thread_id = data.get('thread_id')
        self.message_id = data.get('id')

        self.parent_message_id = None
        self.parent_thread_id = None

        self.sender_id = str(sender.get('id'))

        if bot_id and self.sender_id == bot_id:
            self.is_bot = True
        elif sender.get('type') == 'bot':
            self.is_bot = True
        else:
            self.is_bot = False

        self.sender_name = sender.get('name')
        self.receiver_id = None
        self.receiver_name = None

        self.message_type = body.get('type')
        self.is_markdown_text = body.get('is_markdown_text', False)
        self.mentions = metadata.get('mentions', [])

        self.deleted = data.get('deleted', False)

        media_files = body.get("media", [])
        self.image_urls = [
            file_path for file_path in media_files if is_image(file_path)]
        self.file_paths = [
            file_path for file_path in media_files if not is_image(file_path)]


class APIParentMessage(APIMessage):
    def __init__(self, data: Dict, bot_id: str = None) -> None:
        super().__init__(data, bot_id)
        self.thread_id = data.get('sub_thread', {}).get('id')
        # parent message is the first message in the subthread
        self.message_id = str(0)
        self.parent_thread_id = data.get('thread_id')
        self.parent_message_id = data.get('id')


def convert_to_message(raw_message: Dict) -> CBaseMessage:
    """
    Convert the raw message data from GAPO Webhook to a message object

    Args:
        raw_message (Dict): The raw message data from GAPO Webhook

    Returns:
        CBaseMessage: A message object (ParentThreadMessage, SubThreadMessage, DirectMessage, or WebhookMessage)
    """
    if not raw_message:
        raise ValueError("Invalid message data from GAPO Webhook.")
    message_type = raw_message.get('message', {}).get('thread', {}).get('type')
    if message_type == 'group':
        return ParentThreadMessage(raw_message)
    elif message_type == 'subthread':
        return SubThreadMessage(raw_message)
    elif message_type == 'direct':
        return DirectMessage(raw_message)
    else:
        raise ValueError(
            f"Invalid message type: {message_type}. It must be one of ['group', 'subthread', 'direct'].")
