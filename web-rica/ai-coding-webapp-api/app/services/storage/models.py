from typing import Optional
from schemas.task import CommonStage

class UploadResult(CommonStage):
    upload_path: Optional[str] = None