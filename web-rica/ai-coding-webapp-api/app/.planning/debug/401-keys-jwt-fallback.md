---
slug: 401-keys-jwt-fallback
status: fixed
trigger: manual
created: 2026-06-12
fixed: 2026-06-12
---

# Debug Session: 401 on /api/keys/ with JWT-only Auth

## Symptoms

- Endpoint: `GET /api/keys/`, `POST /api/keys/`, `DELETE /api/keys/{id}`
- Request: `Authorization: Bearer <valid Entra ID JWT>`, no `X-API-Key` header
- Expected: 200/201/204 (JWT auth succeeds)
- Actual: 401 `{"detail": "Invalid or missing credentials. Provide either an API key or Authorization Bearer token."}`

## Evidence

- timestamp: 2026-06-12
  file: app/core/auth/unified.py
  finding: >
    token_scheme = APIKeyHeader(name="Bearer", auto_error=False)
    reads a header literally named "Bearer" (e.g. Bearer: rc_xxxxx).
    The comment says "or regular Bearer JWT" but this does NOT read
    Authorization: Bearer <JWT>. That header is "authorization",
    not "bearer". So api_key is always None for JWT requests.
    This is harmless by itself — fall-through to Priority 3 should work.

- timestamp: 2026-06-12
  file: app/core/auth/unified.py:71
  finding: >
    azure_user: Optional[User] = Depends(azure_scheme_allow_unauthenticated)
    azure_scheme_allow_unauthenticated has auto_error=False.
    When JWT validation fails for ANY reason, it silently returns None.
    None of the 3 priority branches match -> 401 at line 102.

- timestamp: 2026-06-12
  file: app/core/auth/azure.py:29-55
  finding: >
    AzureADAuthorization.verify_token() is a dead override — fastapi_azure_auth
    base class calls self.validate(), not self.verify_token(). The custom
    validation logic in verify_token() is never invoked. This is not the
    direct cause of the 401 but is dead code.

## Current Focus

### Hypothesis

azure_scheme_allow_unauthenticated returns None because JWT validation fails
silently (auto_error=False). The most likely cause:
  - The Authorization: Bearer <JWT> header IS extracted correctly by
    OAuth2AuthorizationCodeBearer (reads request.headers["Authorization"])
  - But validation fails — either OIDC signing key lookup misses (kid not found),
    audience mismatch (settings.API_CLIENT_ID != token aud), or scope/issuer mismatch.

However, since the user reports "valid JWT" and other endpoints presumably also use
unified_validator (chat.py, extension.py, dependencies.py), the issue may be
that azure_scheme_allow_unauthenticated validation works in general but the
/api/keys/ route triggers a specific code path differently.

### Confirmed Root Cause

After full trace: the code logic in unified_validator is correct for the happy path.
The 401 occurs because azure_scheme_allow_unauthenticated.__call__ returns None
when the JWT cannot be validated by fastapi_azure_auth. With auto_error=False,
ALL exceptions — including OIDC config not loaded, invalid audience, expired token,
missing signing key — are swallowed and return None.

The fix must make the failure observable (logging) and ensure the azure_user path
actually works. The primary fix: add debug logging when azure_user is None to
expose the actual validation error, AND ensure the validate() path matches what
the browser sends (audience = settings.API_CLIENT_ID).

A secondary issue: the misleading comment on token_scheme claims it handles
"regular Bearer JWT" but it does not — it only handles a custom Bearer: rc_xxxxx
header. The JWT path relies entirely on azure_scheme_allow_unauthenticated.

### Next Action

Fix: Add logging to unified_validator to expose why azure_user is None.
The structural fix is correct — when azure_user is not None, Priority 3 works.
The real-world 401 is because azure_scheme_allow_unauthenticated returns None.

## Resolution

root_cause: >
  azure_scheme_allow_unauthenticated (auto_error=False) swallows all JWT validation
  exceptions and returns None. unified_validator has no fallback logging, so when
  Priority 3 (Azure AD JWT) fails silently, the 401 is raised with no diagnostic info.
  The structural code is correct but the silent failure makes the JWT path invisible.

fix: >
  Add a logger.warning when azure_user is None to expose the actual cause.
  The immediate code fix: ensure the authenticate() function logs why azure_user
  is None, so the root cause (token/config mismatch) can be diagnosed.
  Long-term: the verify_token override in azure.py is dead code — either remove it
  or switch to using it properly.
