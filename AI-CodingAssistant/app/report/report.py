from fastapi import FastAPI
from fastapi.responses import JSONResponse
from pydantic import BaseModel
import os
import sys
import time
from datetime import datetime
from apscheduler.schedulers.background import BackgroundScheduler
from apscheduler.triggers.cron import CronTrigger
import asyncio
from functools import partial
from google.cloud import storage
from typing import List, Dict
import csv
import tracemalloc

tracemalloc.start()
import asyncio


from dotenv import load_dotenv, find_dotenv
load_dotenv(find_dotenv(), override=True)

if os.getcwd() not in sys.path: sys.path.append(os.getcwd())
from app.database.base import BaseCollection
from app.common.config import logger


report_app = FastAPI()

def get_name_from_mentions(text: str, mentions: List[Dict]):
    for mention in mentions:
        # cut the text from the start to the mention start
        name = ""
        try:
            name = text[mention.get("offset"):mention.get("offset") + mention.get("length")].strip()
        except Exception as e:
            logger.error(f"Error while getting name from mentions: {str(e)}")
            name = ""
        if "opsbot" in name.lower():
            return name
    return ""

def map_response_time_data(list_messages: List[Dict]) -> List[Dict]:
    # create a empty dataframe with column names
    rows = []
    for data in list_messages:
        event_obj = data.get("event", {})
        message_obj = event_obj.get("message", {})
        thread_obj = message_obj.get("thread", {})
        user_obj = event_obj.get("user", {})
        metadata_obj = message_obj.get("metadata", {})
        mentions = metadata_obj.get("mentions", [])
        
        execution_time=data.get("execution_time")
        if isinstance(execution_time, float):
            execution_time = round(execution_time, 2)

        start_time=data.get("start_time")
        if isinstance(start_time, datetime):
            start_time = start_time.strftime("%Y-%m-%d %H:%M:%S")
        
        end_time=data.get("end_time")
        if isinstance(end_time, datetime):
            end_time = end_time.strftime("%Y-%m-%d %H:%M:%S")

        function_name=data.get("function_name")
        event=event_obj.get("event")
        thread_id=event_obj.get("thread_id")
        from_user_id=user_obj.get("id")
        user_name = user_obj.get("name")
        bot_name = get_name_from_mentions(message_obj.get("text", ""), mentions)
        to_bot_id=event_obj.get("to_bot_id")
        message_id=message_obj.get("id")

        parent_id=thread_obj.get("parent_id")
        parent_message_id=thread_obj.get("root_message_id")

        # create a dictionary from above values
        row = {
            "execution_time": execution_time,
            "from_user_name": user_name,
            "to_bot_name": bot_name,
            "start_time": start_time,
            "end_time": end_time,
            "function_name": function_name,
            "event": event,
            "thread_id": thread_id,
            "to_bot_id": to_bot_id,
            "from_user_id": from_user_id,
            "message_id": message_id,
            "parent_id": parent_id,
            "parent_message_id": parent_message_id
        }
        rows.append(row)
    return rows

def map_survey_data(list_messages: List[Dict]) -> List[Dict]:
    rows = []
    for data in list_messages:
        survey_id = data.get("_id")
        thread_id = data.get("thread_id")
        message_id = data.get("message_id")
        sent_at = data.get("sent_at")
        if isinstance(sent_at, datetime):
            sent_at = sent_at.strftime("%Y-%m-%d %H:%M:%S")
        completed_at = data.get("completed_at")
        if isinstance(completed_at, datetime):
            completed_at = completed_at.strftime("%Y-%m-%d %H:%M:%S")
        question = data.get("question")
        feedback = data.get("feedback")
        feedback_id = data.get("feedback_id")
        row = {
            "survey_id": survey_id,
            "thread_id": thread_id,
            "message_id":message_id,
            "sent_at":sent_at,
            "completed_at":completed_at,
            "question":question,
            "feedback":feedback,
            "feedback_id": feedback_id
        }
        rows.append(row)
    return rows

def save_dict_to_csv(rows: List[Dict], save_dir: str) -> str:
    if len(rows) == 0:
        return None
    keys = rows[0].keys()

    file_name =  datetime.now().strftime("%Y-%m-%d-%H-%M-%S") + ".csv"
    file_path = os.path.join(save_dir, file_name)
    os.makedirs(save_dir, exist_ok=True)

    with open(file_path, 'w', newline='', encoding='utf-8') as output_file:
        dict_writer = csv.DictWriter(output_file, keys)
        dict_writer.writeheader()
        dict_writer.writerows(rows)
    return file_path

def create_execution_time_report(from_dt: datetime = None, to_dt: datetime = None):
    collection_timing = BaseCollection("timing")
    # Get the timing data where the end_time is between from_dt and to_dt
    query = {}
    if not from_dt and not to_dt:
        query = {}
    elif not from_dt:
        query = {
            "end_time": {
                "$lte": to_dt
            }
        }
    elif not to_dt:
        query = {
            "end_time": {
                "$gte": from_dt
            }
        }
    else:
        query = {
            "end_time": {
                "$gte": from_dt,
                "$lte": to_dt
            }
        }
    timing_data = collection_timing.find(query)
    data = map_response_time_data(timing_data)
    file_path = save_dict_to_csv(data, RESPONSE_TIME_REPORT_DIR)
    return file_path


def create_survey_report(from_dt: datetime=None, to_dt: datetime = None):
    collection_survey = BaseCollection("survey")
    # Get the survey data where the end_time is between from_dt and to_dt
    if not from_dt and not to_dt:
        query = {}
    elif not from_dt:
        query = {
            "completed_at": {
                "$lte": to_dt
            }
        }
    elif not to_dt:
        query = {
            "completed_at": {
                "$gte": from_dt
            }
        }
    else:
        query = {
            "completed_at": {
                "$gte": from_dt,
                "$lte": to_dt
            },
            "is_completed": True,
        }
    survey_data = collection_survey.find(query)
    data = map_survey_data(survey_data)
    file_path = save_dict_to_csv(data, SURVEY_REPORT_DIR)
    return file_path

    
def upload_data_to_gcs(source_file_name: str, destination_blob_name: str):
    BUCKET_NAME = os.environ.get("GCS_BUCKET_NAME", None)
    CREDENTIALS_PATH = os.environ.get("GOOGLE_APPLICATION_CREDENTIALS", None)

    if BUCKET_NAME is None:
        raise ValueError("GCS_BUCKET_NAME is not set in the environment")
    if CREDENTIALS_PATH is None:
        raise ValueError("GOOGLE_APPLICATION_CREDENTIALS is not set in the environment")
    
    if not os.path.exists(CREDENTIALS_PATH):
        raise FileNotFoundError(f"Credentials file not found at {CREDENTIALS_PATH}")
    

    """Uploads a file to the bucket."""
    # Initialize a client
    storage_client = storage.Client()

    # Get the bucket
    bucket = storage_client.bucket(BUCKET_NAME)

    # Create a blob (an object in the bucket)
    blob = bucket.blob(destination_blob_name)

    # Upload the file to GCS
    blob.upload_from_filename(source_file_name)

    print(f"File {source_file_name} uploaded to {destination_blob_name}.")
    return destination_blob_name


RESPONSE_TIME_REPORT_DIR = os.environ.get("RESPONSE_TIME_REPORT_DIR")
SURVEY_REPORT_DIR = os.environ.get("SURVEY_REPORT_DIR")
GCP_RESPONSE_TIME_REPORT_DIR = os.environ.get("GCP_RESPONSE_TIME_REPORT_DIR")
GCP_SURVEY_REPORT_DIR = os.environ.get("GCP_SURVEY_REPORT_DIR")


def generate_report():
    try:
        # Get the report for the execution time
        rt_report_source_file = create_execution_time_report()
        sv_report_source_file = create_survey_report()

        # split file name from the path
        rt_report_file_name = os.path.split(rt_report_source_file)[1]
        sv_report_file_name = os.path.split(sv_report_source_file)[1]

        full_des_rt_path = os.path.join(GCP_RESPONSE_TIME_REPORT_DIR, rt_report_file_name)
        full_des_sv_path = os.path.join(GCP_SURVEY_REPORT_DIR, sv_report_file_name)
        upload_data_to_gcs(rt_report_source_file, full_des_rt_path)
        upload_data_to_gcs(sv_report_source_file, full_des_sv_path)
        
        msg = f"Report generated successfully!."
        logger.info(msg)
        return JSONResponse(content={"status": "ok", "message": msg})
    except Exception as e:
        exc_type, _, exc_tb = sys.exc_info()
        fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
        msg = f"An error occurred while generating the report. Error: {e}" + \
            f"err_type: {exc_type}, err_file: {fname}, err_line: {exc_tb.tb_lineno}"
        logger.error(msg)
        return JSONResponse(content={"status": "error", "message": msg}, status_code=500)
    

scheduler = BackgroundScheduler()
scheduler.start()

trigger = CronTrigger(
    year="*", month="*", day="*", hour="0", minute="0", second="5"
)
scheduler.add_job(
    generate_report,
    trigger=trigger,
    args=[],
    name="daily generating report",
)

@report_app.get("/")
def trigger_report():
    return generate_report()