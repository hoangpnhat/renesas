from app.common.config import cfg, logger
from app.database.base import BaseCollection
from app.database.schemas import LastMessage, SurveySchema
from app.gapo.create_message import MessageSender
from datetime import datetime, timedelta
import os
import sys
import requests


class SurveyThread:
    def __init__(self):
        self.collection_survey = BaseCollection("survey")
        self.collection_last_message = BaseCollection("last_message")

    def save_last_message(self, thread_id: str, message_id: str, sender_id: str, bot_id: str, message_type: str=None) -> None:
        """
        This function saves the last message id of a thread

        Args:
            thread_id (str): The thread id if it is direct message, or subthread_id if it is a subthread
            message_id (str): The message id
        """
        try:
            if len(self.collection_last_message.find({"thread_id": thread_id})) > 0:
                query = {
                    "thread_id": thread_id,
                    "survey_sent": False
                }
                update = {
                    "$set": {
                        "bot_id": bot_id,
                        "message_id": message_id,
                        "message_sent_at": datetime.now(),
                    }
                }
                if message_type != "reminder":
                    update["$set"]["reminder_sent"] = False
                    update["$set"]["reminder_sent_at"] = None
                self.collection_last_message.update(query, update)
            else:
                data = LastMessage(
                    thread_id=thread_id,
                    message_id=message_id,
                    sender_id=sender_id,
                    bot_id=bot_id,
                    message_type=message_type,
                    message_sent_at=datetime.now(),
                    survey_sent=False,
                    survey_sent_at=None,
                    survey_id=None,
                    reminder_sent=True if message_type == "reminder" else False,
                    reminder_sent_at=datetime.now() if message_type is not None else None
                ).model_dump()
                self.collection_last_message.insert_one(data)
        except Exception as e:
            exc_type, _, exc_tb = sys.exc_info()
            fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
            msg = f"Error: {e}" + f"err_type: {exc_type}, err_file: {fname}, err_line: {exc_tb.tb_lineno}"
            
            logger.error(f"Cannot save the last message id! {msg} \
                          \n thread_id: {thread_id}, message_id: {message_id}")
            
        
    def insert_survey(self, thread_id: str, message_id: str, question: str) -> str:
        """
        Insert a survey record to the database

        Args:
            thread_id (str): The thread id
            message_id (str): The message id
            question (str): The question
        
        Returns:
            str: The inserted id
        """
        survey = SurveySchema(
            thread_id=thread_id,
            message_id=message_id,
            send_at=datetime.now(),
            is_completed=False,
            completed_at=None,
            question=question,
            feedback=None,
            feedback_id=None
        ).model_dump()
        survey_id = self.collection_survey.insert_one(survey)
        return survey_id

    def update_after_sending(self, thread_id: str, message_id: str, survey_id: str) -> None:
        """
        This function updates the last message record after sending the survey

        Args:
            thread_id (str): The thread id
            message_id (str): The message id
            survey_id (str): The survey id
        
        Returns:
            None
        """
        try:
            query = {
                "thread_id": thread_id,
                "message_id": message_id,
            }
            update = {
                "$set": {
                    "survey_sent": True,
                    "survey_sent_at": datetime.now(),
                    "survey_id": survey_id,
                }
            }
            self.collection_last_message.update(query, update)
        except Exception as e:
            exc_type, _, exc_tb = sys.exc_info()
            fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
            msg = f"Error: {e}" + f"err_type: {exc_type}, err_file: {fname}, err_line: {exc_tb.tb_lineno}"
            
            logger.error(f"Cannot update the last message record after sending the survey! {msg} \
                          \n thread_id: {thread_id}, message_id: {message_id}")

    def send_survey(self):
        """
        This function sends the survey to the available threads
        """
        try:
            time_to_minutes_ago = datetime.now() - timedelta(minutes=cfg.min_minutes_to_sent_survey)
            # Query to find records where message_sent_at < now - 30 mins
            query = {
                "reminder_sent_at": {
                    "$lt": time_to_minutes_ago
                },
                "reminder_sent": True,
                "survey_sent": False
            }
            available_threads = self.collection_last_message.find(query)
            msg_sender = MessageSender()

            for thread in available_threads:
                thread_id = str(thread.get("thread_id"))
                bot_id = str(thread.get("bot_id"))
                message_id = str(thread.get("message_id"))
                carousel_cards = [
                                {
                                    "title": cfg.survey_question,
                                    "image_url": "https://gapo-work-image.s3.vn-hcm.zetaby.com/images/478adb11-3d05-4cd9-bf56-b2826c4474cc/blob.jpeg",
                                    "buttons": [{"title": ans, "type": "postback", "payload": id}
                                        for ans, id in zip(cfg.answer_options, cfg.answer_option_ids)]
                                }
                            ]
                result = msg_sender.send_carousel_cards(int(thread_id), int(bot_id), int(message_id), carousel_cards)
                if result:
                    logger.debug(f"Survey sent successfully. Thread id (sub): {thread_id}, message id: {message_id}")
                    survey_id = self.insert_survey(thread_id, message_id, cfg.survey_question)
                    self.update_after_sending(thread_id, message_id, survey_id)

        except Exception as e:
            exc_type, _, exc_tb = sys.exc_info()
            fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
            msg = f"Error: {e}" + f"err_type: {exc_type}, err_file: {fname}, err_line: {exc_tb.tb_lineno}"
            
            logger.error(f"Cannot send the survey! {msg}")

    def update_feedback(self, thread_id: str, feedback: str, feedback_id: str) -> None:
        """
        This function updates the feedback after receiving the feedback
        """

        query = {
            "thread_id": thread_id,
            "is_completed": False
        }

        update = {
            "$set": {
                "feedback": feedback,
                "is_completed": True,
                "completed_at": datetime.now(),
                "feedback_id": feedback_id
            }
        }

        updated_id = self.collection_survey.update(query, update)
        return updated_id
        
    def send_reminder(self):
        time_to_minutes_ago = datetime.now() - timedelta(minutes=cfg.min_minutes_to_sent_survey)
        # Query to find records where message_sent_at < now - 30 mins
        query = {
            "message_sent_at": {
                "$lt": time_to_minutes_ago
            },
            "reminder_sent": False
        }
        msg_sender = MessageSender()
        available_threads = self.collection_last_message.find(query)

        for thread in available_threads:
            thread_id = thread.get("thread_id")
            bot_id = thread.get("bot_id")

            # send text message to subthread using subthread_id, we need to set message_id = None
            message_id = None
            result = msg_sender.send_text_message_to_subthread(thread_id, bot_id, message_id, cfg.reminder_message)
            if result:
                query = {
                    "thread_id": thread_id,
                }
                update = {
                    "$set": {
                        "reminder_sent": True,
                        "reminder_sent_at": datetime.now(),
                        "message_type": "reminder"
                    }
                }
                self.collection_last_message.update(query, update)
                logger.debug(f"Reminder sent successfully. Thread id (sub): {thread_id}")
            else:
                logger.error(f"Cannot send reminder to thread id (sub): {thread_id}")
