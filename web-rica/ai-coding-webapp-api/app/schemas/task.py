from pydantic import BaseModel
from typing import Optional

class CommonStage(BaseModel):
    status: bool = True
    error: Optional[str] = None

class UploadStage(CommonStage):
    upload_path: Optional[str] = None

class TriggerJobStage(CommonStage):
    run_id: Optional[int] = None

class PipelineFailed(CommonStage):
    on_stage: str

