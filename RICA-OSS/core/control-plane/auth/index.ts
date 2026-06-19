import { IdeSettings } from "../..";

// RICA (WorkOS auth severed): upstream built a WorkOS
// authorize-redirect URL pointing at api.workos.com with the Continue Hub
// client_id and ${env.APP_URL}tokens/callback as the redirect target.
// Called from core.ts auth/getAuthUrl handler when the GUI requests the
// "Open token page in browser" flow. RICA never opens that flow — auth
// goes through EntraID via rica/adapters/auth/EntraIDAuthProvider.ts and
// VsCodeMessenger's getControlPlaneSessionInfo handler — so this function
// has no live caller in the Rica build. Throwing keeps the
// `auth/getAuthUrl` IPC channel responsive (the protocol type is still
// declared in core/protocol/core.ts and core.ts:1135 still binds the
// handler) but makes any stray invocation surface a clear error instead
// of silently constructing a WorkOS authorize URL.
//
// `_ideSettingsPromise` and `_useOnboarding` are kept in the signature so
// the upstream `auth/getAuthUrl` protocol contract is unchanged — only
// the body is severed.
export async function getAuthUrlForTokenPage(
  _ideSettingsPromise: Promise<IdeSettings>,
  _useOnboarding: boolean,
): Promise<string> {
  throw new Error(
    "Rica: getAuthUrlForTokenPage is not configured for the Renesas backend. " +
      "EntraID sign-in is handled by rica/adapters/auth/EntraIDAuthProvider.ts.",
  );
}
