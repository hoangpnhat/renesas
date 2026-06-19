from fastapi import FastAPI, Response, BackgroundTasks
from contextlib import asynccontextmanager
import os
import sys
from langfuse.decorators import observe, langfuse_context
from langfuse.callback import CallbackHandler
from typing import Dict
from apscheduler.schedulers.background import BackgroundScheduler
import asyncio
from functools import partial
from langchain_core.messages import BaseMessage
if os.getcwd() not in sys.path:
    sys.path.append(os.getcwd())
from app.gapo.create_message import MessageSender
from app.gapo.get_message import MessageGetter
from app.common.config import cfg, logger
from app.gapo.survey import SurveyThread
from app.common.timing import timing
from app.gapo.message import DirectMessage, ParentThreadMessage, SubThreadMessage, APIMessage, APIParentMessage
from app.gapo.message import convert_to_message
from app.chatbot.agents.orchestrator import generate_answer

@asynccontextmanager
async def lifespan(app: FastAPI):
    # Load the ML model
    # ml_models["answer_to_everything"] = fake_answer_to_everything_ml_model
    yield
    # Clean up the ML models and release the resources
    # ml_models.clear()
    pass

# gapo_app = FastAPI(lifespan=lifespan)

gapo_app = FastAPI()
langfuse_handler = CallbackHandler()
message_sender = MessageSender()
message_getter = MessageGetter()
survey = SurveyThread()


def survey_scheduler():
    """
    This function sends the survey to the available threads
    """
    try:
        survey.send_reminder()
        survey.send_survey()
        return Response(status_code=200, content="Survey sent successfully")
    except Exception as e:
        exc_type, _, exc_tb = sys.exc_info()
        fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
        msg = f"Error while sending survey: {e}. Error type: {exc_type}, file name {fname}, line no {exc_tb.tb_lineno}"
        logger.error(msg)
        return Response(status_code=500, content=msg)


# Create a subprocess to call function send_survey every 3 minutes
SCHERDULED_INTERVAL = os.environ.get("SCHEDULER_INTERVAL_MINS", 5)
# Set up the scheduler
scheduler = BackgroundScheduler()
scheduler.add_job(survey_scheduler, 'interval',
                  seconds=int(SCHERDULED_INTERVAL)*60)
scheduler.start()


def scheduler_shutdown():
    scheduler.shutdown()


gapo_app.add_event_handler("shutdown", scheduler_shutdown)


@gapo_app.post("/chatbot247")
@timing
@observe()
async def handle_webhook(event: Dict):
    """
    Webhook to handle messages from Gapo and send responses back once the user sends a message to the bot

    Args:
        event (GapoMessage): The event from Gapo

    Returns:
        Response: A response object
    """
    logger.debug(f"Received event: {event}")
    loop = asyncio.get_event_loop()

    event_type = event.get("event")
    msg_type = event.get("message", {}).get('type')

    # check if this is the feedback message from user
    if msg_type == "quick_reply":
        feedback_id = str(event.get("message", {}).get('payload'))
        thread_id = str(event.thread_id)
        feedback = event.get("message", {}).get('text')
        loop.run_in_executor(None, survey.update_feedback,
                             thread_id, feedback, feedback_id)
        return Response(status_code=200)

    # Check the event type
    if event_type == 'message_created':
        message = convert_to_message(event)
        chat_history = []
        chat_history_wt_image = []
        parent_message = None
        # Get chat history
        if isinstance(message, (SubThreadMessage, DirectMessage)):
            if isinstance(message, SubThreadMessage):
                raw_parent_msg = message_getter.get_parent_message(
                    message.parent_thread_id, message.parent_message_id)
                parent_message = APIParentMessage(
                    raw_parent_msg).to_langchain_message(apply_image=True)
                parent_message_wt_image = APIParentMessage(
                    raw_parent_msg).to_langchain_message(apply_image=False)

            raw_history = message_getter.get_messages(message.thread_id)

            chat_history = [APIMessage(msg).to_langchain_message(
                apply_image=True) for msg in raw_history]

            chat_history_wt_image = [APIMessage(msg).to_langchain_message(
                apply_image=False) for msg in raw_history]
            if isinstance(parent_message, BaseMessage):
                chat_history.insert(0, parent_message)
            if isinstance(parent_message_wt_image, BaseMessage):
                chat_history_wt_image.insert(0, parent_message_wt_image)
            logger.debug(f"Chat history: {chat_history_wt_image}")

        # If there is an image in the parent message, add it to the chat history
        if len(message.image_urls) > 0 and len(chat_history) == 0:
            chat_history = [message.to_langchain_message(apply_image=True)]

        # Get response from the Agent
        answer, json_object = generate_answer(
            message, chat_history, chat_history_wt_image)
        mentions = []
        if not json_object or len(json_object.get('mention', [])) == 0:
            mentions = []
        else:
            mentions = [
                {
                    "pic_gapo_name": user.get('pic_gapo_name', "NULL"),
                    "pic_gapo_id": user.get('pic_gapo_id', "xxxxxxxx")
                }
                for user in json_object.get('mention', [])
            ]

        # Send message back to user
        session_id = "NOT_FOUND_SESSION_ID"
        if isinstance(message, DirectMessage):
            receiver_id = message.sender_id
            bot_id = message.receiver_id
            response = message_sender.send_text_message_to_user(
                int(receiver_id), int(bot_id), answer)
            session_id = str(response.get('thread_id'), None) or session_id
        else:
            parent_thread_id = message.parent_thread_id
            parent_message_id = message.parent_message_id
            response = message_sender.send_text_message_to_subthread(thread_id=int(parent_thread_id),
                                                                     bot_id=int(
                                                                         message.receiver_id),
                                                                     message_id=int(
                                                                         parent_message_id),
                                                                     message=answer,
                                                                     mention=mentions)

            session_id = str(response.get('sub_thread_id', None)) or session_id

            # save the last message to send survey
            survey.save_last_message(thread_id=str(session_id),
                                     message_id=str(response.get('message_id')),
                                     sender_id=str(message.receiver_id),
                                     bot_id=str(message.receiver_id),
                                     message_type="reply")

        loop.run_in_executor(None,
                             partial(langfuse_context.update_current_trace,
                                     name="Gapo-Chatbot247",
                                     user_id=event.get("from_user_id"),
                                     session_id=session_id,
                                     metadata={
                                         "sub_thread_id": session_id,
                                     }))

    else:
        # Unsupported event type
        event_type = event.get("event", "NOT_FOUND_EVENT_TYPE")
        logger.critical(
            f"Unsupported event type {event_type}. The body is {event}")
        return Response(status_code=400, content=f"Unsupported event type {event_type}")
    loop.run_in_executor(None, langfuse_handler.flush)
    # Return a success response
    return Response(status_code=200)


@gapo_app.get("/send_survey")
def send_survey():
    survey_scheduler()
    return Response(content="Triggered survey successfully", status_code=200)
