#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
import os
DEFAULT_PAGE_NUMBER = 0
DEFAULT_NUMBER_OF_ITEM_PAGE = 10
LANGUAGE = {"en": "English", "ja": "Japanese"}
PROMPT_LANGUAGE = {
    "ja": """会話の本質を捉えた簡潔で魅力的なタイトルを作成してください：
- 8語未満
- 会話の核心を反映
- 専門的で簡潔な表現
- 読み手の興味を引く
- タイトル文字列のみ返してください。

会話の文脈：\"{PROMPT}\"
""",
    "en": """Create a concise, engaging title that:
- Captures the essence of the conversation
- Is less than 8 words long
- Uses title case
- Highlights the key theme or purpose
- Only return title string

Conversation context: \"{PROMPT}\" """,
}
PROMPT_TITLE = """You are a professional translator and title creator specialized in cross-cultural communication.

Original Prompt: {PROMPT}
Target Language: {LANGUAGE}
Desired Tone: Professional

Generate a title that:
- Precisely captures the essence of the original prompt
- Adapts to linguistic and cultural nuances of the target language
- Maintains the original intent and professional quality.
- It only have 10-15 words."""
TOKEN = os.environ.get("AZURE_AD_TOKEN", "")

ALLOWED_MIME_TYPES = {
    # Documents
    "application/pdf": "pdf",

    # PowerPoint
    "application/vnd.openxmlformats-officedocument.presentationml.presentation": "pptx",
    "application/vnd.ms-powerpoint": "ppt",
}
