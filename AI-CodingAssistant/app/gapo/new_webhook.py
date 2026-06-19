from fastapi import FastAPI, Response
from pydantic import BaseModel
import os
import sys
from langfuse.decorators import observe, langfuse_context
from langfuse.callback import CallbackHandler
import time
from typing import Dict
from apscheduler.schedulers.asyncio import AsyncIOScheduler
import asyncio
from functools import partial
from langchain_core.messages import BaseMessage
if os.getcwd() not in sys.path:
    sys.path.append(os.getcwd())
from app.gapo.create_message import AsyncMessageSender
from app.gapo.get_message import AsyncMessageGetter
from app.common.config import cfg, logger
from app.gapo.survey import AsyncSurveyThread
from app.common.timing import timing
from app.gapo.message import DirectMessage, ParentThreadMessage, SubThreadMessage, APIMessage, APIParentMessage
from app.gapo.message import convert_to_message
from app.chatbot.agents.orchestrator import generate_answer_async

gapo_app = FastAPI()
langfuse_handler = CallbackHandler()
message_sender = AsyncMessageSender()
message_getter = AsyncMessageGetter()
survey = AsyncSurveyThread()

async def survey_scheduler():
    """
    This function sends the survey to the available threads
    """
    try:
        await asyncio.gather(
            survey.send_reminder(),
            survey.send_survey()
        )
        return Response(status_code=200, content="Survey sent successfully")
    except Exception as e:
        exc_type, _, exc_tb = sys.exc_info()
        fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
        msg = f"Error while sending survey: {e}. Error type: {exc_type}, file name {fname}, line no {exc_tb.tb_lineno}"
        logger.error(msg)
        return Response(status_code=500, content=msg)

SCHEDULED_INTERVAL = int(os.environ.get("SCHEDULER_INTERVAL_MINS", 5))
scheduler = AsyncIOScheduler()
scheduler.add_job(survey_scheduler, 'interval', minutes=SCHEDULED_INTERVAL)
scheduler.start()

@gapo_app.on_event("shutdown")
async def scheduler_shutdown():
    scheduler.shutdown()

@gapo_app.post("/chatbot247")
@timing
@observe()
async def handle_webhook(event: Dict):
    logger.debug(f"Received event: {event}")

    event_type = event.get("event")
    msg_type = event.get("message", {}).get('type')

    if msg_type == "quick_reply":
        feedback_id = str(event.get("message", {}).get('payload'))
        thread_id = str(event.thread_id)
        feedback = event.get("message", {}).get('text')
        asyncio.create_task(survey.update_feedback(thread_id, feedback, feedback_id))
        return Response(status_code=200)

    if event_type == 'message_created':
        message = convert_to_message(event)
        chat_history = []
        chat_history_wt_image = []
        parent_message = None

        if isinstance(message, (SubThreadMessage, DirectMessage)):
            if isinstance(message, SubThreadMessage):
                raw_parent_msg = await message_getter.get_parent_message(message.parent_thread_id, message.parent_message_id)
                parent_message = APIParentMessage(raw_parent_msg).to_langchain_message(apply_image=True)
                parent_message_wt_image = APIParentMessage(raw_parent_msg).to_langchain_message(apply_image=False)
                
            raw_history = await message_getter.get_messages(message.thread_id)

            chat_history = [APIMessage(msg).to_langchain_message(apply_image=True) for msg in raw_history]
            chat_history_wt_image = [APIMessage(msg).to_langchain_message(apply_image=False) for msg in raw_history]
            
            if isinstance(parent_message, BaseMessage):
                chat_history.insert(0, parent_message)
            if isinstance(parent_message_wt_image, BaseMessage):
                chat_history_wt_image.insert(0, parent_message_wt_image)
            
            logger.debug(f"Chat history: {chat_history_wt_image}")

        if len(message.image_urls) > 0 and len(chat_history) == 0:
            chat_history = [message.to_langchain_message(apply_image=True)]
        
        answer, json_object = await generate_answer_async(message, chat_history, chat_history_wt_image)
        mentions = []
        if json_object and len(json_object.get('mention', [])) > 0:
            mentions = [
                {
                    "pic_gapo_name": user.get('pic_gapo_name', "NULL"),
                    "pic_gapo_id": user.get('pic_gapo_id', "xxxxxxxx")
                }
                for user in json_object.get('mention', [])
            ]

        session_id = "NOT_FOUND_SESSION_ID"
        if isinstance(message, DirectMessage):
            receiver_id = message.sender_id
            bot_id = message.receiver_id
            response = await message_sender.send_text_message_to_user(int(receiver_id), int(bot_id), answer)
            session_id = str(response.get('thread_id', None) or session_id)
        else:
            parent_thread_id = message.parent_thread_id
            parent_message_id = message.parent_message_id
            response = await message_sender.send_text_message_to_subthread(
                thread_id=int(parent_thread_id),
                bot_id=int(message.receiver_id),
                message_id=int(parent_message_id),
                message=answer,
                mention=mentions
            )
            session_id = str(response.get('sub_thread_id', None) or session_id)

        asyncio.create_task(
            langfuse_context.update_current_trace(
                name="Gapo-Chatbot247",
                user_id=event.get("from_user_id"),
                session_id=session_id,
                metadata={
                    "sub_thread_id": session_id,
                }
            )
        )

    else:
        event_type = event.get("event", "NOT_FOUND_EVENT_TYPE")
        logger.critical(f"Unsupported event type {event_type}. The body is {event}")
        return Response(status_code=400, content=f"Unsupported event type {event_type}")
    
    return Response(status_code=200)

@gapo_app.get("/send_survey")
async def send_survey():
    await survey_scheduler()
    return Response(content="Triggered survey successfully", status_code=200)

