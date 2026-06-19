from const.exceptions import ERRORS


class ApplicationException(Exception):
    def __init__(self, error_key: str | None = None, status_code: int = 500, detail: str | None = None, **custom_details):
        self.error_key = error_key
        error = ERRORS.get(error_key, {}) if error_key else {}

        self.status_code = error.get("status_code", status_code)
        self.detail: str = detail or str(error.get("detail", "Undefined business error occurred."))

        if custom_details:
            self.detail = self.detail.format(**custom_details)

    def to_dict(self):
        return {"status_code": self.status_code,
                "error_key": self.error_key,
                "detail": self.detail}

    def __str__(self):
        return f"[{self.status_code}] {self.detail}"
