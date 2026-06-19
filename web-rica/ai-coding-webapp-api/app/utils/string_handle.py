import re


def capitalize_first_letter(s: str) -> str:
    if not s:
        return ""

    # Find the first alphabetical character and capitalize it
    def replacer(match):
        return match.group(0).upper()

    return re.sub(r"[a-zA-Z]", replacer, s, count=1)
