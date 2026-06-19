from typing import Any
from exceptions.base import BaseError


class DataBaseException(BaseError):
    def __init__(self, message: str, error_code: int, *args: object) -> None:
        super().__init__(message, error_code, *args)

    def __str__(self) -> str:
        return f"Got database exception:\n{self.message}. Error code {self.error_code}"


class RedisException(BaseError):
    def __init__(self, message: str, error_code: int, *args: object) -> None:
        super().__init__(message, error_code, *args)

    def __str__(self) -> str:
        return f"Got Redis exception:\n{self.message}. Error code {self.error_code}"


class NotExistedRecord(DataBaseException):
    def __init__(self, id: str, instance_name: str, *args: object) -> None:
        self.id = id
        self.instance_name = instance_name

    def __str__(self) -> str:
        return f"Record {self.instance_name} with id: {self.id} does not exist in database."


class ExistedRecord(DataBaseException):
    def __init__(self, id: str, instance_name: str, *args: object) -> None:
        self.id = id
        self.instance_name = instance_name

    def __str__(self) -> str:
        return (
            f"Record with {self.instance_name} with id: {self.id} has already existed."
        )


class CanNotFindRecord(Exception):
    def __init__(self, message: str = "Cannot find the resource") -> None:
        super().__init__(message)
        self.message = message

    def __str__(self) -> str:
        return self.message
