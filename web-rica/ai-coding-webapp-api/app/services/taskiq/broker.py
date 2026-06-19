import importlib
from inspect import getmembers, isfunction
from logging import getLogger
from types import FunctionType
from typing import Union, Any, AsyncGenerator
from urllib.parse import quote_plus

from redis.asyncio import Redis
from taskiq import AckableMessage
from taskiq import TaskiqEvents, TaskiqState
from taskiq_redis import RedisStreamBroker

from core.config import settings
from caching.redis import normalize_redis_uri, _redis_client as redis_cli
from services.taskiq import tasks

logger = getLogger("taskiq.redis_broker")


def load_tasks() -> dict[str, tuple[str, FunctionType]]:
    task_dict = {}
    module = importlib.import_module(tasks.__name__)
    for method_name, method in getmembers(module, isfunction):
        if method_name.startswith("sync"):
            simple_task_name = method_name
            task_dict[simple_task_name] = (f"{method.__module__}:{method_name}", method)
    return task_dict


def convert_redis_connection_details(host, port, password, ssl=True, abort_connect=False, **kwargs):
    password = quote_plus(password)
    redis_url = f"redis://:{password}@{host}:{port}"
    query_params = []
    if ssl:
        query_params.append("ssl=true")
    if not abort_connect:
        query_params.append("abortConnect=false")

    if query_params:
        redis_url += '?' + '&'.join(query_params)

    return redis_url


class CustomBroker(RedisStreamBroker):
    def __init__(self):
        assert settings.REDIS_URI is not None
        super().__init__(
            url=normalize_redis_uri(settings.REDIS_URI),
            queue_name="taskiq-rica",
            consumer_group_name="taskiq-rica")

        self.tasks = {}

    def register_tasks(self) -> None:
        """Store the task in the internal dictionary."""

        tasks = load_tasks()
        for simple_name, (real_name, func) in tasks.items():
            self.tasks[simple_name] = real_name
            self.register_task(func=func, task_name=real_name)

    async def listen(self) -> AsyncGenerator[AckableMessage, None]:
        """Listen to incoming messages."""

        async with Redis(connection_pool=self.connection_pool) as redis_conn:
            while True:
                logger.debug("Starting fetching new messages")
                fetched = await redis_conn.xreadgroup(
                    self.consumer_group_name,
                    self.consumer_name,
                    {
                        self.queue_name: ">",
                        **self.additional_streams,
                    },
                    block=self.block,
                    noack=False,
                    count=self.count,
                )
                for _, msg_list in fetched:
                    for msg_id, msg in msg_list:
                        logger.debug("Received message: %s", msg)
                        yield AckableMessage(
                            data=msg[b"data"],
                            ack=self._ack_generator(msg_id),
                        )

                logger.debug("Starting fetching unacknowledged messages")
                for stream in [self.queue_name, *self.additional_streams.keys()]:
                    lock = redis_conn.lock(
                        f"autoclaim:{self.consumer_group_name}:{stream}",
                    )
                    if await lock.locked():
                        continue
                    async with lock:
                        # Get detailed list of pending messages
                        # Different Redis clients return different structures, so let's adapt
                        try:
                            # Try the full format first
                            pending = await redis_conn.xpending(  # type: ignore[call-arg]
                                stream,
                                self.consumer_group_name,
                                "-",
                                "+",
                                self.unacknowledged_batch_size,
                            )
                        except TypeError:
                            # If that fails, try just getting all pending messages
                            pending = await redis_conn.xpending(
                                stream,
                                self.consumer_group_name,
                            )
                            # If pending is a dict with a 'pending' key, it's a summary
                            if isinstance(pending, dict) and 'pending' in pending:
                                # No pending messages or wrong format
                                continue

                        pending_ids = []

                        # Handle different formats of the pending results
                        if pending:
                            if isinstance(pending, list):
                                for p_info in pending:
                                    # Common format: [message_id, consumer, idle_time, deliveries]
                                    if isinstance(p_info, list) and len(p_info) >= 3:
                                        msg_id, _, idle_time = p_info[0], p_info[1], p_info[2]
                                        if idle_time >= self.idle_timeout:
                                            pending_ids.append(msg_id)
                                    # Dict format: {'message_id': ..., 'consumer': ..., 'idle': ...}
                                    elif isinstance(p_info, dict) and 'idle' in p_info and 'message_id' in p_info:
                                        if p_info['idle'] >= self.idle_timeout:
                                            pending_ids.append(p_info['message_id'])

                        if pending_ids:
                            try:
                                # Claim messages that meet our idle time criteria
                                claimed_msgs = await redis_conn.xclaim(
                                    stream,
                                    self.consumer_group_name,
                                    self.consumer_name,
                                    self.idle_timeout,
                                    pending_ids,
                                )

                                logger.debug(
                                    "Found %d pending messages in stream %s",
                                    len(claimed_msgs),
                                    stream,
                                )

                                for msg_id, msg in claimed_msgs:
                                    logger.debug("Received message: %s", msg)
                                    yield AckableMessage(
                                        data=msg[b"data"],
                                        ack=self._ack_generator(msg_id, stream),  # type: ignore[call-arg]
                                    )
                            except Exception as e:
                                logger.error(f"Error claiming messages: {e}")

    async def send_task(self, task_name: str, *args, **kwargs) -> Union[None, Any]:
        """Find and execute the task based on the name."""

        real_name = self.tasks.get(task_name)

        func = self.find_task(real_name or "")
        if func is None:
            raise ValueError(f"Task with name '{task_name}' not found.")

        await func.kiq(*args, **kwargs)
        return None


broker = CustomBroker()
# Register tasks after broker creation to avoid circular imports
broker.register_tasks()


@broker.on_event(TaskiqEvents.WORKER_STARTUP)
async def startup(state: TaskiqState) -> None:
    # Store connection pool on startup for later use.
    state.redis = redis_cli
    logger.info("Taskiq worker started successfully")


@broker.on_event(TaskiqEvents.WORKER_SHUTDOWN)
async def shutdown(state: TaskiqState) -> None:
    # Here we close our pool on shutdown event.
    await state.redis.close()
    logger.info("Taskiq worker shutdown completed")
