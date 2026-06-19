import os
from fastapi import APIRouter, HTTPException
from fastapi.responses import FileResponse


router = APIRouter()


@router.get("/{full_path:path}")
async def serve_react(full_path: str):
    # return full_path
    static_file = f"./static/{full_path}"
    if os.path.isfile(static_file):
        # Set correct content type for JavaScript modules
        if full_path.endswith(".js") or full_path.endswith(".mjs"):
            return FileResponse(static_file, media_type="application/javascript")
        elif full_path.endswith(".css"):
            return FileResponse(static_file, media_type="text/css")
        return FileResponse(static_file)

    # For all other routes, serve index.html
    index_file = "./static/index.html"
    if os.path.exists(index_file):
        return FileResponse(index_file, media_type="text/html")

    raise HTTPException(status_code=404, detail="File not found")
