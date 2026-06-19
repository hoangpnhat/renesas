import asyncio
from functools import wraps
import time
from datetime import datetime

from app.database.base import BaseCollection
timing_collection = BaseCollection("timing")


def timing(func):
    @wraps(func)
    async def wrapper(*args, **kwargs):
        start_dt = datetime.now()
        start_time = time.perf_counter()
        result = await func(*args, **kwargs)  # Make sure to await the function if it's async
        end_time = time.perf_counter()
        execution_time = end_time - start_time
        end_dt = datetime.now()
        timing_object = {
            "function_name": func.__name__,
            "execution_time": execution_time,
            "start_time": start_dt,
            "end_time": end_dt,
            "event": kwargs.get('event', None)
        }
        await asyncio.get_event_loop().run_in_executor(None, timing_collection.insert_one, timing_object)
        return result
    return wrapper