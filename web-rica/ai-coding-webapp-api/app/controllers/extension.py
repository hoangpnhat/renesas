#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
from typing import Optional
from fastapi import HTTPException, status
import httpx

from core.config import settings


async def get_service_principal_access_token(version: Optional[str] = None):
    if settings.EXT_VERSION:
        if not version:
            raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail="Version is required")
        if version not in settings.EXT_VERSION:
            raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST,
                                detail="Version from your assistant is not match with latest version. Please update your assistant to the latest version. Latest version is " + str(settings.EXT_VERSION))
    """Retrieve a service principal access token from Entra ID to interact with Databricks resources."""
    tenant_id = settings.EXT_AD_TENANT_ID
    client_id = settings.EXT_AD_CLIENT_ID
    client_secret = settings.EXT_AD_CLIENT_SECRET
    databricks_client_id = settings.DATABRICKS_CLIENT_ID
    scope = databricks_client_id + "/.default"
    token_url = f"https://login.microsoftonline.com/{tenant_id}/oauth2/v2.0/token"
    payload = {
        "grant_type": "client_credentials",
        "client_id": client_id,
        "client_secret": client_secret,
        "scope": scope
    }
    async with httpx.AsyncClient() as client:
        response = await client.post(token_url, data=payload)
        response.raise_for_status()
        return response.json()
