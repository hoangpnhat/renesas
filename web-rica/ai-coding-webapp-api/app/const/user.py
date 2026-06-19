#  Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
from enum import Enum
from typing import Literal


class Action(Enum):
    LOGIN = "LOGIN"


ENTRA_ROLES = Literal["Admin_GR", "Admin_EC", "Admin_Ringi", "Admin", "User"]
ENTRA_ROLE_PREFIX = "Admin"
