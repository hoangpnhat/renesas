from db.session import check_health_db
from .chat import *
from .databricks import *
from .user import *


async def check_health():
    response = await check_health_db()
    return {"message": response}
