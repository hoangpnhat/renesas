import functools
from typing import Callable, Union, Any, TypeVar, Awaitable, cast
from time import time
from loguru import logger

_R = TypeVar('_R')

from caching.redis import get_redis_client
from controllers.user import UserController

SENSITIVE_KEYS = {"azure_token", "password", "access_token", 'refresh_token'}
non_critical_errors = ["DuplicateKeyError", "CredentialError"]

def mask_sensitive_data(data: Any) -> dict[Any, Any]:
    masked = {}
    if isinstance(data, dict):
        for k, v in data.items():
            if k == 'azure_user':
                masked[k] = v.preferred_username
            else:
                masked[k] = '***' if k in SENSITIVE_KEYS else v
        return masked
    elif hasattr(data, "__dict__"):
        for attr, value in data.__dict__.items():
            masked[attr] = "****" if attr in SENSITIVE_KEYS else value
        return masked
    return cast(dict[Any, Any], data)

def log_api(func: Callable) -> Union[Any]:
    @functools.wraps(func)
    async def api_log_decorator(*args, **kwargs):
        api_name = func.__name__
        filtered_kwargs = mask_sensitive_data(kwargs)
        filtered_args = [
            mask_sensitive_data(arg) if isinstance(arg, dict) else arg for arg in args[1:]
        ]
        start_time = time()
        logger.log("API_INFO", f"{api_name} starts with input - {filtered_kwargs}, {filtered_args}")
        try:
            response = await func(*args, **kwargs)
            process_time = time() - start_time
            logger.log("API_INFO", f"{api_name} completed successfully in {process_time:.2f}s, type: {response.__class__.__name__}")
            return response
        except Exception as e:
            logger.log("API_INFO", f"{api_name} request failed, error: {str(e)}")
            raise e
    return api_log_decorator


def cache_result(prefix: str, expire: int = 3600):
    def decorator(func: Callable[..., Awaitable[_R]]) -> Callable[..., Awaitable[_R]]:
        @functools.wraps(func)
        async def wrapper(*args: Any, **kwargs: Any) -> _R:
            try:
                element = args[1]
            except Exception:
                element = kwargs.get('element')

            # generate the cache key
            key = f"{prefix}-{element.id}" if hasattr(element, 'id') else f"{prefix}-{element}"

            # check if the key is cached in Redis
            redis_client = get_redis_client()
            if cached_result := await redis_client.get(key):
                return cast(_R, cached_result)

            # if not cached, call the original function and cache the result
            result = await func(*args, **kwargs)

            # cache the result for future use
            await redis_client.set(key, result, expire)
            return result

        return cast(Callable[..., Awaitable[_R]], wrapper)

    return decorator


def invalidate_endpoint_cache(prefix: str, path_param: str | None = None):
    def decorator(func: Callable) -> Callable:
        @functools.wraps(func)
        async def wrapper(*args, **kwargs):
            # Extract ID from path parameter or function args
            identity = None
            if path_param and path_param in kwargs:
                identity = kwargs[path_param]

            # Invalidate cache
            if identity is not None:
                cache_key = f"{prefix}-{identity}"
                redis_client = get_redis_client()
                await redis_client.delete(cache_key)

            # Execute the endpoint function
            return await func(*args, **kwargs)

        return wrapper

    return decorator

def require_user_validator(attribute_name: str = 'verified_user', return_fields: list = ['id']):
    def decorator(func: Callable[..., Awaitable[_R]]) -> Callable[..., Awaitable[_R]]:
        @functools.wraps(func)
        async def wrapper(self: Any, *args: Any, **kwargs: Any) -> _R:

            # Check if the instance has the required attribute
            if not hasattr(self, attribute_name):
                raise AttributeError(
                    f"{self.__class__.__name__} must have a '{attribute_name}' attribute"
                )

            # Perform user validate
            user = getattr(self, attribute_name)
            if user:
                user = await UserController(verified_user=user).validate_curr_user()

            new_kwargs = kwargs.copy()
            for field in return_fields:
                if hasattr(user, field):
                    new_kwargs[field] = getattr(user, field)

            # with the injected extra fields
            return await func(self, *args, **new_kwargs)
        return cast(Callable[..., Awaitable[_R]], wrapper)
    return decorator
