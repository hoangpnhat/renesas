class BaseError(Exception):
    def __init__(self, message: str, error_code: int, *args: object) -> None:
        super().__init__(message, error_code, *args)
        self.error_code = error_code
        self.message = message

    def __str__(self) -> str:
        return f"{self.message}\nError Code : {self.error_code}"
