#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

import traceback

import socketio
from fastapi.encoders import jsonable_encoder

import controllers
from core.loguru import logger
from core.config import settings
from core.auth.token import token_strategy
from caching.redis import get_redis_client, normalize_redis_uri
from exceptions.https_exception import DataBricksTitleErrors
from schemas.chat import (
    MessageContentCreate,
    MessageContentRequestSocketDTO,
)
from utils.helpers import datetime_now_sec, normalize_id

# Channel name for Redis pub/sub
SOCKETIO_CHANNEL = "socketio"

# Create Redis client manager for cross-process communication
if not settings.REDIS_URI:
    raise RuntimeError(
        "REDIS_URI is not configured. "
        "Socket.IO requires a Redis connection — set REDIS_URI in your environment."
    )
redis_manager = socketio.AsyncRedisManager(
    normalize_redis_uri(settings.REDIS_URI),
    channel=SOCKETIO_CHANNEL
)

# Create a Socket.IO server instance with Redis manager
sio = socketio.AsyncServer(
    async_mode="asgi",
    cors_allowed_origins="*",  # Adjust this to your client's origin
    client_manager=redis_manager,
)


# Define Socket.IO events
@sio.event
async def connect(sid, environ, auth):
    try:
        # Authenticate user on connection
        token = auth.get('token') if auth else None
        if not token:
            logger.error(f"No token provided for connection: {sid}")
            return False

        # Authenticate using token_strategy
        verified_user, user = await token_strategy.authenticate(access_token=token)

        if user is None:
            logger.error(f"No user record found for connection: {sid}")
            return False

        # Store user in session
        await sio.save_session(sid, {
            'user': user,
            'verified_user': verified_user
        })

        # Join user to their personal room
        user_room = f"user_{user.id}"
        await sio.enter_room(sid, user_room)

        logger.info(f"Client connected and authenticated: {sid}, user: {user.email if user else 'unknown'}, room: {user_room}")

    except Exception as e:
        logger.error(f"Authentication failed for {sid}: {str(e)}")
        return False


#
# @sio.event
# async def disconnect(sid):
#     logger.info(f"Client disconnected: {sid}")


@sio.on("message")
async def message(sid, request_body: MessageContentRequestSocketDTO):
    try:
        # Retrieve user from session
        session = await sio.get_session(sid)
        user = session.get('user')

        if not user:
            logger.error(f"No user found in session for {sid}")
            await sio.emit(
                "message_error",
                {"message": "User not authenticated"},
                to=sid,
            )
            return False

        request_body = MessageContentRequestSocketDTO.model_validate(request_body)
        dialog_id = request_body.dialog_id
        if dialog_id:
            redis = get_redis_client()

            user_chat_schema = MessageContentCreate(
                **jsonable_encoder(request_body),
                user_id=user.id,
                timestamp=datetime_now_sec(),
            )

            result = await controllers.prompt_socket(
                dialog_id=dialog_id,
                user_prompt=request_body,
                sio=sio,
                redis=redis,
                sid=sid,
            )

            if not result or not result.content:
                raise Exception("No content found")

            # TODO: Reduce number of time called to db
            user_chat = await controllers.databricks.create_context_and_prompt(
                sio=sio,
                dialog_id=dialog_id,
                db_in=user_chat_schema,
                sid=sid,
            )

            bot_chat_schema = MessageContentCreate(
                content=result.content,
                role="assistant",
                retrieved_docs=result.retrieved_docs,
                user_id=user.id,
                dialog_id=normalize_id(dialog_id),
                relate_to=user_chat.relate_to,
                timestamp=datetime_now_sec(),
            )

            bot_chat = await controllers.create_context_and_prompt(
                sio=sio,
                sid=sid,
                dialog_id=dialog_id,
                db_in=bot_chat_schema,
                ref_id=user_chat.id,
            )

            if request_body.content is None:
                await sio.emit('error', {'message': 'Content is required'}, to=sid)
                return
            title = await controllers.update_title(
                content=request_body.content,
                dialog_id=dialog_id,
                language=request_body.language,
            )
            if title is not None:
                await sio.emit(
                    "update_title_dialog", {"data": jsonable_encoder(title)}, to=sid
                )
    except DataBricksTitleErrors as e:
        await sio.emit("title_error", {"message": str(e)})
    except Exception as e:
        logger.error(str(e))
        traceback.print_exc()
        await sio.emit(
            "message_error",
            {"data": jsonable_encoder(request_body), "message": str(e)},
            to=sid,
        )
        return False



