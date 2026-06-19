---
slug: 401-keys-audience
status: awaiting_human_verify
trigger: manual
created: 2026-06-12
updated: 2026-06-12
---

# Debug Session: 401-keys-audience

## Symptoms

- `fastapi_azure_auth` logs: "Token contains invalid claims. Audience doesn't match"
- Error: "Invalid or missing credentials. Provide either an API key or Authorization Bearer token."
- Status 401, request from browser (Edge/Chrome)
- Route: GET /api/keys/
- Branch: improvement/cli-api-key (recent commit: feat: add API key authentication alongside Entra ID)

## Current Focus

hypothesis: azure.py uses settings.azure.APP_CLIENT_ID (falls back to "test_app_client_id" — not set in .env) for JWT audience validation, but all tokens are issued for settings.API_CLIENT_ID (eb30e2c5-...). The audience never matches, azure_user returns None (auto_error=False), and the unified_validator raises 401.

reasoning_checkpoint:
  hypothesis: "JWT audience validation in azure.py uses settings.azure.APP_CLIENT_ID which defaults to 'test_app_client_id' because APP_CLIENT_ID is not in .env, causing InvalidAudienceError on every Azure JWT"
  confirming_evidence:
    - ".env has API_CLIENT_ID=eb30e2c5-3a7f-4dd7-99aa-6a1a1cd665c3 but no APP_CLIENT_ID entry"
    - "AzureSettings.APP_CLIENT_ID defaults to 'test_app_client_id'"
    - "azure.py line 59+63: app_client_id=settings.azure.APP_CLIENT_ID or ''"
    - "fastapi_azure_auth validates with audience=self.app_client_id — so validation uses 'test_app_client_id'"
    - "main.py uses settings.API_CLIENT_ID for Swagger OAuth2 clientId and scopes — token aud = eb30e2c5-..."
    - "fastapi_azure_auth auto_error=False on azure_scheme_allow_unauthenticated → returns None on any auth error"
    - "unified_validator: azure_user=None → falls through to raise HTTP 401"
  falsification_test: "If APP_CLIENT_ID were set in .env to a valid GUID matching token aud, the audience error would disappear"
  fix_rationale: "azure.py should use settings.API_CLIENT_ID (which IS set in .env) instead of settings.azure.APP_CLIENT_ID (which is not set and falls back to 'test_app_client_id'). AzureSettings.APP_CLIENT_ID is an orphaned duplicate field."
  blind_spots: "If the token's aud is actually 'api://eb30e2c5-...' rather than just 'eb30e2c5-...', the audience match still requires the api:// prefix. But fastapi_azure_auth uses app_client_id directly as audience string — need to confirm the token aud format matches the raw GUID."

next_action: Change azure.py to use settings.API_CLIENT_ID instead of settings.azure.APP_CLIENT_ID

## Evidence

- timestamp: 2026-06-12
  checked: core/auth/azure.py
  found: AzureADAuthorization instantiated with app_client_id=settings.azure.APP_CLIENT_ID
  implication: audience for JWT validation = AzureSettings.APP_CLIENT_ID

- timestamp: 2026-06-12
  checked: core/config.py AzureSettings
  found: APP_CLIENT_ID defaults to "test_app_client_id"
  implication: If not set in .env, every Azure JWT gets InvalidAudienceError

- timestamp: 2026-06-12
  checked: app/.env
  found: API_CLIENT_ID=eb30e2c5-3a7f-4dd7-99aa-6a1a1cd665c3 present; APP_CLIENT_ID NOT present
  implication: AzureSettings.APP_CLIENT_ID = "test_app_client_id" at runtime

- timestamp: 2026-06-12
  checked: main.py swagger_ui_init_oauth
  found: clientId=settings.API_CLIENT_ID; scopes=api://settings.API_CLIENT_ID/user_impersonation
  implication: Browser gets token with aud=eb30e2c5-... but validation uses "test_app_client_id" → mismatch

- timestamp: 2026-06-12
  checked: fastapi_azure_auth/auth.py __call__ method
  found: InvalidAudienceError caught at line 232, raises Unauthorized; outer except at line 257 with auto_error=False returns None
  implication: azure_user=None returned to unified_validator; falls through to HTTP 401

## Eliminated

- hypothesis: API key auth fallback has a logic bug preventing it from being reached
  evidence: unified_validator correctly checks x_api_key and api_key before azure_user; the issue is azure_user=None when it should return a valid user for browser JWT requests
  timestamp: 2026-06-12

## Resolution

root_cause: azure.py uses settings.azure.APP_CLIENT_ID (not set in .env, defaults to "test_app_client_id") as the JWT audience for fastapi_azure_auth validation, while all Azure tokens are issued for settings.API_CLIENT_ID (eb30e2c5-3a7f-4dd7-99aa-6a1a1cd665c3). Every Azure JWT gets InvalidAudienceError; with auto_error=False, azure_user=None, and unified_validator raises 401.

fix: Change both AzureADAuthorization instantiations in azure.py to use settings.API_CLIENT_ID instead of settings.azure.APP_CLIENT_ID

files_changed: [app/core/auth/azure.py, app/core/config.py]
status: resolved
