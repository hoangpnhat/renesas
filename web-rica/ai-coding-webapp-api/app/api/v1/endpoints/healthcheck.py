from fastapi import APIRouter, HTTPException, status
from fastapi.encoders import jsonable_encoder
from caching.redis import get_redis_client
from db.session import check_health_db

from controllers.databricks import get_dbx_prompt_response
from core.config import settings
from const.chat import TEST_MESSAGE_QUESTION
from schemas.chat import MessageContentSchema

router = APIRouter()


@router.get("/")
async def healthcheck_api():
    try:
        return {"status": "healthy", "message": "Successfully"}
    except ConnectionError:
        raise HTTPException(status_code=503, detail="Error")
    except Exception as e:
        raise HTTPException(
            status_code=500, detail=f"Error checking connection: {str(e)}"
        )


@router.get("/mongodb")
async def check_mongodb_connection():
    try:
        await check_health_db()
        return {"status": "healthy", "message": "Successfully connected to Mongodb"}
    except ConnectionError:
        raise HTTPException(status_code=503, detail="Cannot connect to Mongodb server")
    except Exception as e:
        raise HTTPException(
            status_code=500, detail=f"Error checking Mongodb connection: {str(e)}"
        )


@router.get("/redis")
async def check_redis_connection():
    try:
        # Ping Redis to check connection
        redis_client = get_redis_client()
        await redis_client.get("Something")
        await redis_client.ping()
        return {"status": "healthy", "message": "Successfully connected to Redis"}
    except ConnectionError:
        raise HTTPException(status_code=503, detail="Cannot connect to Redis server")
    except Exception as e:
        raise HTTPException(
            status_code=500, detail=f"Error checking Redis connection: {str(e)}"
        )


@router.post("/databricks")
async def check_databricks_health():
    try:
        input_prompt = MessageContentSchema(
            content=TEST_MESSAGE_QUESTION,
            role="user",
            # retrieved_docs=["code_base", "document"],
        )
        result = await get_dbx_prompt_response(input_prompt)

        dbx_endpoint = (settings.databricks.DATABRICKS_ENDPOINT or "").format(
            endpoint_name=settings.databricks.DATABRICKS_ENDPOINT_NAME
        )
        return {
            "response": jsonable_encoder(result),
            "url": dbx_endpoint,
        }
    except Exception as e:
        dbx_endpoint = (settings.databricks.DATABRICKS_ENDPOINT or "").format(
            endpoint_name=settings.databricks.DATABRICKS_ENDPOINT_NAME
        )
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=str(e) + f"\n{dbx_endpoint}",
        )
