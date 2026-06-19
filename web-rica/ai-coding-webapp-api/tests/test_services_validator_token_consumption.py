#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

import pytest
from datetime import datetime, timezone
from pydantic import ValidationError
from odmantic import ObjectId

from app.services.validator.token_consumption import ConsumptionRecord


# A valid model_id used across all tests that only care about client_timestamp
_VALID_MODEL_ID = str(ObjectId())


def _make_record(client_timestamp) -> ConsumptionRecord:
    """Helper: build a minimal valid ConsumptionRecord with the given client_timestamp."""
    return ConsumptionRecord(
        model_id=_VALID_MODEL_ID,
        tokens_in=10,
        tokens_out=1,
        client_timestamp=client_timestamp,
    )


@pytest.mark.unit
class TestParseClientTimestamp:
    """Unit tests for ConsumptionRecord.parse_client_timestamp validator."""

    # ------------------------------------------------------------------ #
    # Integer inputs                                                       #
    # ------------------------------------------------------------------ #

    def test_int_returned_as_is(self):
        """Plain integer is stored without modification."""
        ts = 1744963200000
        record = _make_record(ts)
        assert record.client_timestamp == ts

    def test_int_zero_accepted(self):
        """Zero is a valid integer timestamp (epoch origin)."""
        record = _make_record(0)
        assert record.client_timestamp == 0

    def test_negative_int_accepted(self):
        """Negative integers (pre-epoch) pass through without rejection."""
        record = _make_record(-1000)
        assert record.client_timestamp == -1000

    # ------------------------------------------------------------------ #
    # Numeric string inputs                                                #
    # ------------------------------------------------------------------ #

    def test_numeric_string_milliseconds(self):
        """Numeric string is parsed as int and used as-is."""
        record = _make_record("1744963200000")
        assert record.client_timestamp == 1744963200000

    def test_numeric_string_zero(self):
        """Numeric string '0' is accepted."""
        record = _make_record("0")
        assert record.client_timestamp == 0

    def test_numeric_string_negative(self):
        """Negative numeric string is accepted."""
        record = _make_record("-1000")
        assert record.client_timestamp == -1000

    # ------------------------------------------------------------------ #
    # ISO 8601 string inputs                                               #
    # ------------------------------------------------------------------ #

    def test_iso8601_utc_z_suffix(self):
        """ISO 8601 with Z suffix is converted to milliseconds."""
        record = _make_record("2026-04-07T10:30:00.000Z")
        expected = int(datetime(2026, 4, 7, 10, 30, 0, tzinfo=timezone.utc).timestamp() * 1000)
        assert record.client_timestamp == expected

    def test_iso8601_utc_plus_offset(self):
        """ISO 8601 with +00:00 offset produces the same result as Z suffix."""
        ts_z = _make_record("2026-04-07T10:30:00.000Z").client_timestamp
        ts_offset = _make_record("2026-04-07T10:30:00.000+00:00").client_timestamp
        assert ts_z == ts_offset

    def test_iso8601_positive_timezone_offset(self):
        """ISO 8601 with positive UTC offset is converted to UTC milliseconds correctly."""
        # 2026-04-07T17:30:00+07:00 == 2026-04-07T10:30:00Z
        record = _make_record("2026-04-07T17:30:00+07:00")
        expected = int(datetime(2026, 4, 7, 10, 30, 0, tzinfo=timezone.utc).timestamp() * 1000)
        assert record.client_timestamp == expected

    def test_iso8601_negative_timezone_offset(self):
        """ISO 8601 with negative UTC offset is converted to UTC milliseconds correctly."""
        # 2026-04-07T05:30:00-05:00 == 2026-04-07T10:30:00Z
        record = _make_record("2026-04-07T05:30:00-05:00")
        expected = int(datetime(2026, 4, 7, 10, 30, 0, tzinfo=timezone.utc).timestamp() * 1000)
        assert record.client_timestamp == expected

    def test_iso8601_no_timezone_assumes_utc(self):
        """ISO 8601 without timezone info is treated as UTC."""
        record = _make_record("2026-04-07T10:30:00")
        expected = int(datetime(2026, 4, 7, 10, 30, 0, tzinfo=timezone.utc).timestamp() * 1000)
        assert record.client_timestamp == expected

    def test_iso8601_with_milliseconds_precision(self):
        """ISO 8601 with sub-second precision is preserved in the millisecond output."""
        record = _make_record("2026-04-07T10:30:00.123Z")
        expected = int(datetime(2026, 4, 7, 10, 30, 0, 123000, tzinfo=timezone.utc).timestamp() * 1000)
        assert record.client_timestamp == expected

    def test_iso8601_date_only_no_time(self):
        """ISO 8601 date-only string (no time component) is treated as midnight UTC."""
        record = _make_record("2026-04-07")
        expected = int(datetime(2026, 4, 7, 0, 0, 0, tzinfo=timezone.utc).timestamp() * 1000)
        assert record.client_timestamp == expected

    # ------------------------------------------------------------------ #
    # Invalid inputs                                                       #
    # ------------------------------------------------------------------ #

    def test_arbitrary_string_raises_validation_error(self):
        """Non-numeric, non-ISO string raises ValidationError."""
        with pytest.raises(ValidationError) as exc_info:
            _make_record("not-a-timestamp")
        assert "client_timestamp" in str(exc_info.value)

    def test_partial_iso_string_raises_validation_error(self):
        """Malformed ISO 8601 string raises ValidationError."""
        with pytest.raises(ValidationError):
            _make_record("2026-13-99T25:61:00Z")

    def test_empty_string_raises_validation_error(self):
        """Empty string raises ValidationError."""
        with pytest.raises(ValidationError):
            _make_record("")

    def test_none_raises_validation_error(self):
        """None raises ValidationError (field is required and non-nullable)."""
        with pytest.raises(ValidationError):
            _make_record(None)

    def test_float_raises_validation_error(self):
        """Float input raises ValidationError (only int and str accepted)."""
        with pytest.raises(ValidationError):
            _make_record(1744963200.123)

    def test_list_raises_validation_error(self):
        """List input raises ValidationError."""
        with pytest.raises(ValidationError):
            _make_record([1744963200000])

    def test_missing_field_raises_validation_error(self):
        """Omitting client_timestamp entirely raises ValidationError."""
        with pytest.raises(ValidationError) as exc_info:
            ConsumptionRecord(
                model_id=_VALID_MODEL_ID,
                tokens_in=10,
                tokens_out=1,
            )
        assert "client_timestamp" in str(exc_info.value)
