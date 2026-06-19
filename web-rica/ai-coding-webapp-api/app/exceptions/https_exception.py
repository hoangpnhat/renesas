from exceptions.base import BaseError


class DataBricksException(BaseError):
    def __init__(self, message: str, error_code: int, *args: object) -> None:
        super().__init__(message, error_code, *args)

    def __str__(self):
        return f"Databricks ran into this issue.\nError: {self.message}\n.Code: {self.error_code}"


class DataBricksTitleErrors(BaseError):
    def __init__(self, message: str, error_code: int, *args: object) -> None:
        super().__init__(message, error_code, *args)

    def __str__(self):
        return f"Databricks ran into this issue.\nError: {self.message}\n.Code: {self.error_code}"
