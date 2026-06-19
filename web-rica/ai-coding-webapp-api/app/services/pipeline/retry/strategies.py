"""
Retry strategies for pipeline failures

Provides exponential backoff with jitter for retry delays.
"""

from dataclasses import dataclass
import random


@dataclass
class RetryStrategy:
    """Exponential backoff with jitter"""
    base_delay: float = 1.0
    multiplier: float = 2.0
    max_delay: float = 30.0
    jitter: float = 0.1  # 10% jitter

    def get_delay(self, attempt: int) -> float:
        """Calculate delay for given attempt number"""
        delay = self.base_delay * (self.multiplier ** attempt)
        delay = min(delay, self.max_delay)

        # Add jitter
        jitter_range = delay * self.jitter
        delay += random.uniform(-jitter_range, jitter_range)

        return max(0, delay)


@dataclass
class RetryContext:
    """Context for retry decisions"""
    page_num: int
    stage: str
    error: str
    attempt: int
    max_attempts: int

    @property
    def can_retry(self) -> bool:
        return self.attempt < self.max_attempts

    @property
    def is_retriable_error(self) -> bool:
        """Check if error is retriable"""
        non_retriable = [
            "unsupported file type",
            "invalid pdf",
            "corrupted file",
            "permission denied",
        ]
        error_lower = self.error.lower()
        return not any(term in error_lower for term in non_retriable)
