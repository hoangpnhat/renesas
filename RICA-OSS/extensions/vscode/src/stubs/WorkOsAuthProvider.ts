// Phase 5.1.23 (Group 5 — WorkOS auth severed): the original WorkOS PKCE
// flow + JWT refresh + getControlPlaneSessionInfo helper hit api.workos.com
// + ${CONTROL_PLANE_URL}/auth/refresh on Continue's hub. RICA replaces the
// whole runtime with EntraIDAuthProvider via rica/vscode/integration/auth.ts
// — RicaAuthProvider wraps EntraIDAuthProvider, getRicaControlPlaneSessionInfo
// re-exports the EntraID variant, and VsCodeMessenger wires both in. Nothing
// in the codebase imports WorkOsAuthProvider any more (verified Jun 10 with
// `grep -rn "from .*WorkOsAuthProvider"` returning zero hits across
// extensions/vscode, core, rica, gui, binary), so this file is dead code.
//
// Kept on disk as a stub rather than deleted because (a) the .vitest.ts
// sister-file still references the class shape and removal would require
// touching the test harness in the same commit, (b) any future upstream
// merge that re-introduces an import will fail loudly with the throw below
// instead of silently calling api.workos.com, and (c) Phase 5.5 / Phase 6
// is the right place to decide whether to delete the stub + its test.
//
// Every public method throws "Rica: not configured" — defence in depth
// against a stray instantiation. The actual auth runtime lives in
// rica/adapters/auth/EntraIDAuthProvider.ts.

import {
  AuthenticationProvider,
  AuthenticationProviderAuthenticationSessionsChangeEvent,
  AuthenticationSession,
  Disposable,
  EventEmitter,
  ExtensionContext,
} from "vscode";

import { ControlPlaneSessionInfo } from "core/control-plane/AuthTypes";

import { UriEventHandler } from "./uriHandler";

const RICA_AUTH_DISABLED_ERROR =
  "Rica: WorkOsAuthProvider is not configured for the Renesas backend. " +
  "Use rica/vscode/integration/auth.ts → RicaAuthProvider (EntraID) instead.";

interface ContinueAuthenticationSession extends AuthenticationSession {
  refreshToken: string;
  expiresInMs: number;
  loginNeeded: boolean;
}

export class WorkOsAuthProvider implements AuthenticationProvider, Disposable {
  // Static surface preserved so any leftover caller fails on instance use,
  // not on field access. `hasAttemptedRefresh` resolves immediately so a
  // caller that awaits it (config loading would lock waiting forever) can
  // proceed and then hit the throw on the next real call.
  public static EXPIRATION_TIME_MS = 1000 * 60 * 15;
  public static REFRESH_INTERVAL_MS = 1000 * 60 * 10;
  public static useOnboardingUri: boolean = false;
  public static hasAttemptedRefresh: Promise<void> = Promise.resolve();

  private _sessionChangeEmitter =
    new EventEmitter<AuthenticationProviderAuthenticationSessionsChangeEvent>();

  constructor(
    _context: ExtensionContext,
    _uriHandler: UriEventHandler,
  ) {
    throw new Error(RICA_AUTH_DISABLED_ERROR);
  }

  get onDidChangeSessions() {
    return this._sessionChangeEmitter.event;
  }

  get redirectUri(): string {
    throw new Error(RICA_AUTH_DISABLED_ERROR);
  }

  get ideRedirectUri(): string {
    throw new Error(RICA_AUTH_DISABLED_ERROR);
  }

  public async getSessions(
    _scopes?: string[],
  ): Promise<ContinueAuthenticationSession[]> {
    throw new Error(RICA_AUTH_DISABLED_ERROR);
  }

  public async createSession(
    _scopes: string[],
  ): Promise<ContinueAuthenticationSession> {
    throw new Error(RICA_AUTH_DISABLED_ERROR);
  }

  public async removeSession(_sessionId: string): Promise<void> {
    throw new Error(RICA_AUTH_DISABLED_ERROR);
  }

  public async refreshSessions(): Promise<void> {
    throw new Error(RICA_AUTH_DISABLED_ERROR);
  }

  public async dispose(): Promise<void> {
    // Allow disposal without throwing — safer if a future code path holds a
    // reference and unwinds during shutdown.
    this._sessionChangeEmitter.dispose();
  }
}

// Phase 5.1.23: top-level export preserved for type-compat. Runtime callers
// go through getRicaControlPlaneSessionInfo (rica/vscode/integration/auth.ts)
// which re-exports the EntraID variant under the same name.
export async function getControlPlaneSessionInfo(
  _silent: boolean,
  _useOnboarding: boolean,
): Promise<ControlPlaneSessionInfo | undefined> {
  throw new Error(RICA_AUTH_DISABLED_ERROR);
}
