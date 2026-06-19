from abc import ABC, abstractmethod

from .models import ValidationResult

class BaseValidator(ABC):
    @abstractmethod
    async def run(self) -> ValidationResult:
        pass