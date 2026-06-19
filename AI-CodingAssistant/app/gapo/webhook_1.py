import os
import sys
from typing import Dict
from contextlib import asynccontextmanager
from functools import partial
import asyncio

from fastapi import FastAPI, Response, BackgroundTasks, Depends
from apscheduler.schedulers.background import BackgroundScheduler
from langfuse.decorators import observe, langfuse_context
from langfuse.callback import CallbackHandler

if os.getcwd() not in sys.path:
    sys.path.append(os.getcwd())

from app.gapo.create_message import MessageSender
from app.gapo.get_message import MessageGetter
from app.common.config import cfg, logger
from app.gapo.survey import SurveyThread
from app.common.timing import timing
from app.gapo.message import DirectMessage, ParentThreadMessage, SubThreadMessage, APIMessage, APIParentMessage, convert_to_message
from app.chatbot.agents.orchestrator import generate_answer

@asynccontextmanager
async def lifespan(app: FastAPI):
    yield

gapo_app = FastAPI(lifespan=lifespan)

def get_langfuse_handler():
    return langfuse_context.get_current_langchain_handler()

def get_message_sender():
    return MessageSender()

def get_message_getter():
    return MessageGetter()

def get_survey():
    return SurveyThread()


@gapo_app.on_event("startup")
async def startup_event():
    scheduler = BackgroundScheduler()
    scheduler.add_job(survey_scheduler, 'interval', seconds=int(os.environ.get("SCHEDULER_INTERVAL_MINS", 5)) * 60)
    scheduler.start()
    gapo_app.state.scheduler = scheduler

@gapo_app.on_event("shutdown")
async def shutdown_event():
    gapo_app.state.scheduler.shutdown()

async def survey_scheduler():
    survey = get_survey()
    try:
        await survey.send_reminder()
        await survey.send_survey()
        return Response(status_code=200, content="Survey sent successfully")
    except Exception as e:
        logger.error(f"Error while sending survey: {e}", exc_info=True)
        return Response(status_code=500, content=str(e))

@gapo_app.post("/chatbot247")
@timing
@observe()
async def handle_webhook(
    event: Dict,
    background_tasks: BackgroundTasks,
    langfuse_handler: CallbackHandler = Depends(get_langfuse_handler),
    message_sender: MessageSender = Depends(get_message_sender),
    message_getter: MessageGetter = Depends(get_message_getter),
    survey: SurveyThread = Depends(get_survey)
):
    logger.debug(f"Received event: {event}")

    event_type = event.get("event")
    msg_type = event.get("message", {}).get('type')

    if msg_type == "quick_reply":
        await survey.update_feedback(
            str(event.get("thread_id")),
            event.get("message", {}).get('text'),
            str(event.get("message", {}).get('payload'))
        )
        return Response(status_code=200)

    if event_type != 'message_created':
        logger.critical(f"Unsupported event type {event_type}. The body is {event}")
        return Response(status_code=400, content=f"Unsupported event type {event_type}")

    message = convert_to_message(event)
    chat_history, chat_history_wt_image = await get_chat_history(message, message_getter)

    answer, json_object, issue_id, issue_name = await generate_answer(message, chat_history, chat_history_wt_image)
    mentions = get_mentions(json_object)

    session_id = await send_response(message, answer, mentions, message_sender, survey)

    background_tasks.add_task(update_langfuse_context,
                              str(event.get("from_user_id", "None")),
                              session_id,
                              langfuse_handler,
                              issue_id=issue_id,
                              issue_name=issue_name)

    return Response(status_code=200)

async def get_chat_history(message, message_getter):
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

        if parent_message:
            chat_history.insert(0, parent_message)
            chat_history_wt_image.insert(0, parent_message_wt_image)

    elif message.image_urls:
        chat_history = [message.to_langchain_message(apply_image=True)]

    logger.debug(f"Chat history: {chat_history_wt_image}")
    return chat_history, chat_history_wt_image

def get_mentions(json_object):
    if not json_object or not json_object.get('mention'):
        return []
    return [
        {
            "pic_gapo_name": user.get('pic_gapo_name', "NULL"),
            "pic_gapo_id": user.get('pic_gapo_id', "xxxxxxxx")
        }
        for user in json_object.get('mention', [])
    ]

async def send_response(message, answer, mentions, message_sender, survey, background_tasks: BackgroundTasks):
    if isinstance(message, DirectMessage):
        response = await message_sender.send_text_message_to_user(
            int(message.sender_id), int(message.receiver_id), answer
        )
        session_id = str(response.get('thread_id', "NOT_FOUND_SESSION_ID"))
    else:
        response = await message_sender.send_text_message_to_subthread(
            thread_id=int(message.parent_thread_id),
            bot_id=int(message.receiver_id),
            message_id=int(message.parent_message_id),
            message=answer,
            mention=mentions
        )
        session_id = str(response.get('sub_thread_id', "NOT_FOUND_SESSION_ID"))

        background_tasks.add_task(
            survey.save_last_message,
            thread_id=session_id,
            message_id=str(response.get('message_id')),
            sender_id=str(message.receiver_id),
            bot_id=str(message.receiver_id),
            message_type="reply"
        )

    return session_id

async def update_langfuse_context(user_id: str, 
                                  session_id: str, 
                                  langfuse_handler: CallbackHandler,
                                  **kwargs):
    await langfuse_handler.update_current_trace(
        name="Gapo-Chatbot247",
        user_id=user_id,
        session_id=session_id,
        metadata={"sub_thread_id": session_id, **kwargs}
    )

@gapo_app.get("/send_survey")
async def send_survey():
    await survey_scheduler()
    return Response(content="Triggered survey successfully", status_code=200)