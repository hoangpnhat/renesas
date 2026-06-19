import { getControlPlaneEnvSync } from "../config/env";
import crypto from "crypto";
// @ts-ignore - Runtime dependency, types not needed for compilation
import fetch from "node-fetch";
// @ts-ignore - Runtime dependency, types not needed for compilation
import { v4 as uuidv4 } from "uuid";
import {
  authentication,
  AuthenticationProvider,
  AuthenticationProviderAuthenticationSessionsChangeEvent,
  AuthenticationSession,
  Disposable,
  env,
  EventEmitter,
  ExtensionContext,
  ProgressLocation,
  Uri,
  window,
} from "vscode";

import { ControlPlaneSessionInfo, HubEnv, isHubEnv } from "./AuthTypes";
import { PromiseAdapter, promiseFromEvent } from "./promiseUtils";
import { SecretStorage } from "./SecretStorage";
import { UriEventHandler } from "./uriHandler";
import brandConfig from "../../branding/brand.json";

interface BackendTokenResponse {
  token_type: string;
  expires_in: number;
  access_token: string;
  ext_expires_in: number;
}
const AUTH_NAME = "Rica";

// Lazy load controlPlaneEnv from VS Code settings instead of hardcoding
function getControlPlaneEnvFromSettings(): HubEnv {
  const vscodeModule = require("vscode") as typeof import("vscode");
  const settings = vscodeModule.workspace.getConfiguration("rica");
  const env = settings.get<
    "staging" | "production" | "local" | "test" | "none"
  >("continueTestEnvironment", "production");
  return getControlPlaneEnvSync(env) as HubEnv;
}

function getSessionsSecretKey(): string {
  const env = getControlPlaneEnvFromSettings();
  return `${env.AUTH_TYPE}.sessions`;
}

// Exported for backward compatibility with RicaProxy
export const SESSIONS_SECRET_KEY = getSessionsSecretKey();

// Function to generate a random string of specified length
function generateRandomString(length: number): string {
  const possibleCharacters =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
  let randomString = "";
  for (let i = 0; i < length; i++) {
    const randomIndex = Math.floor(Math.random() * possibleCharacters.length);
    randomString += possibleCharacters[randomIndex];
  }
  return randomString;
}

// Function to generate a code challenge from the code verifier

async function generateCodeChallenge(verifier: string) {
  // Create a SHA-256 hash of the verifier
  const hash = crypto.createHash("sha256").update(verifier).digest();

  // Convert the hash to a base64 URL-encoded string
  const base64String = hash
    .toString("base64")
    .replace(/\+/g, "-")
    .replace(/\//g, "_")
    .replace(/=+$/, "");

  return base64String;
}

interface RicaAuthenticationSession extends AuthenticationSession {
  refreshToken: string;
  expiresInMs: number;
  loginNeeded: boolean;
  entraAccessToken?: string; // Store original Entra token
}

export class EntraIDAuthProvider implements AuthenticationProvider, Disposable {
  private _sessionChangeEmitter =
    new EventEmitter<AuthenticationProviderAuthenticationSessionsChangeEvent>();
  private _disposable: Disposable;
  private _pendingStates: string[] = [];
  private retryAttempts = 0;
  private _codeExchangePromises = new Map<
    string,
    { promise: Promise<string>; cancel: EventEmitter<void> }
  >();
  private _refreshIntervalId: NodeJS.Timeout | undefined;
  private _refreshTimeoutId: NodeJS.Timeout | undefined;

  private static EXPIRATION_TIME_MS = 1000 * 60 * 12 * 60; // 12 hours
  // DEBUG: Set to 2 minutes for testing - REMOVE IN PRODUCTION
  // private static EXPIRATION_TIME_MS = 1000 * 60 * 1; // 2 minutes for testing

  private secretStorage: SecretStorage;

  constructor(
    private readonly context: ExtensionContext,
    private readonly _uriHandler: UriEventHandler,
  ) {
    this._disposable = Disposable.from(
      authentication.registerAuthenticationProvider(
        AUTH_NAME,
        AUTH_NAME,
        this,
        { supportsMultipleAccounts: false },
      ),
      window.registerUriHandler(this._uriHandler),
    );

    this.secretStorage = new SecretStorage(context);
  }
  private _scheduleRefresh(expiresInMs: number) {
    console.log(
      `[DEBUG] Scheduling token refresh in ${Math.round((expiresInMs * 2) / 3 / 1000 / 60)} minutes`,
    );

    // Clear any existing scheduled refresh
    if (this._refreshTimeoutId) {
      console.log("[DEBUG] Clearing existing refresh timeout");
      clearTimeout(this._refreshTimeoutId);
      this._refreshTimeoutId = undefined;
    }

    const refreshInterval = (expiresInMs * 2) / 3;

    this._refreshTimeoutId = setTimeout(async () => {
      console.log("[DEBUG] Token refresh timer triggered!");
      try {
        await this._refreshSessions();
        this.retryAttempts = 0; // reset if successful
        console.log("[DEBUG] Token refresh completed successfully");
      } catch (e) {
        console.error(`[DEBUG] Refresh failed: ${e}`);
        this.retryAttempts++;
        // exponential backoff (max 30 minutes)
        const backoff = Math.min(
          30 * 60 * 1000,
          60 * 1000 * 2 ** (this.retryAttempts - 1),
        );
        this._scheduleRefresh(backoff);
      }
    }, refreshInterval);
  }
  private async exchangeTokenWithBackend(entraAccessToken: string): Promise<{
    mdpToken: string;
    expiresInMs: number;
  }> {
    try {
      const controlPlaneEnv = getControlPlaneEnvFromSettings();
      // Call your backend API to exchange Entra token for MDP token
      const response = await fetch(
        `${controlPlaneEnv.APP_URL}/api/extension/token`,
        {
          method: "POST",
          headers: {
            "Content-Type": "application/json",
            Authorization: `Bearer ${entraAccessToken}`,
          },
          body: JSON.stringify({
            version: brandConfig.product.version,
          }),
          // You can also send additional data if needed
        },
      );

      if (!response.ok) {
        const errorText = await response.text();
        throw new Error(
          `Backend token exchange failed: ${response.status} - ${errorText}`,
        );
      }

      const data: BackendTokenResponse =
        (await response.json()) as unknown as BackendTokenResponse;

      return {
        mdpToken: data.access_token,
        expiresInMs: data.expires_in
          ? data.expires_in * 1000
          : this.getExpirationTimeMs(data.access_token),
      };
    } catch (error) {
      console.error("Error exchanging token with backend:", error);
      throw new Error(`Failed to exchange token with backend: ${error}`);
    }
  }

  private decodeJwt(jwt: string): Record<string, any> | null {
    try {
      const decodedToken = JSON.parse(
        Buffer.from(jwt.split(".")[1], "base64").toString(),
      );
      return decodedToken;
    } catch (e: any) {
      console.warn(`Error decoding JWT: ${e}`);
      return null;
    }
  }

  private getExpirationTimeMs(jwt: string): number {
    const decodedToken = this.decodeJwt(jwt);
    if (!decodedToken) {
      return EntraIDAuthProvider.EXPIRATION_TIME_MS;
    }
    return decodedToken.exp && decodedToken.iat
      ? (decodedToken.exp - decodedToken.iat) * 1000
      : EntraIDAuthProvider.EXPIRATION_TIME_MS;
  }

  private getJwtField(jwt: string, field: string): number | string | undefined {
    const decodedToken = this.decodeJwt(jwt);
    if (!decodedToken) {
      return undefined;
    }

    return decodedToken[field];
  }

  private jwtIsExpiredOrInvalid(jwt: string): boolean {
    const decodedToken = this.decodeJwt(jwt);
    if (!decodedToken) {
      return true;
    }
    return decodedToken.exp * 1000 < Date.now();
  }

  private async debugAccessTokenValidity(jwt: string, refreshToken: string) {
    const expiredOrInvalid = this.jwtIsExpiredOrInvalid(jwt);
    if (expiredOrInvalid) {
      console.debug("Invalid JWT");
    } else {
      console.debug("Valid JWT");
    }
  }

  public async validateAndRefreshIfNeeded(): Promise<boolean> {
    try {
      const sessions = await this.getSessions();
      if (!sessions.length) return false;

      const session = sessions[0];
      // Check if token will expire in next 5 minutes
      const decodedToken = this.decodeJwt(session.accessToken);
      if (!decodedToken) return false;

      const expiresAt = decodedToken.exp * 1000;
      const now = Date.now();
      const fiveMinutes = 5 * 60 * 1000;

      if (expiresAt - now < fiveMinutes) {
        console.log("Token expiring soon, refreshing proactively...");
        await this._refreshSessions();
      }
      return true;
    } catch (e) {
      console.error("Token validation failed:", e);
      return false;
    }
  }

  public async ensureValidToken(): Promise<boolean> {
    const sessions = await this.getSessions();
    if (!sessions.length) {
      console.log("[DEBUG] No sessions found");
      return false;
    }

    const session = sessions[0];

    // Debug: Check token expiry
    const decodedToken = this.decodeJwt(session.accessToken);
    if (decodedToken) {
      const expiresAt = new Date(decodedToken.exp * 1000);
      const now = new Date();
      console.log(
        `[DEBUG] Token expires at: ${expiresAt.toISOString()}, Current time: ${now.toISOString()}`,
      );
    }

    // Check if MDP token is expired
    if (this.jwtIsExpiredOrInvalid(session.accessToken)) {
      console.log("[DEBUG] Token is expired, triggering refresh...");
      try {
        await this._refreshSessions();
        console.log("[DEBUG] Token refresh completed successfully");
        return true;
      } catch (e) {
        console.error("[DEBUG] Failed to refresh expired token:", e);
        return false;
      }
    }

    console.log("[DEBUG] Token is still valid");
    return true;
  }

  private async storeSessions(value: RicaAuthenticationSession[]) {
    const data = JSON.stringify(value, null, 2);
    await this.secretStorage.store(getSessionsSecretKey(), data);
  }

  public async getSessions(
    scopes?: string[],
  ): Promise<RicaAuthenticationSession[]> {
    const data = await this.secretStorage.get(getSessionsSecretKey());
    if (!data) {
      return [];
    }

    try {
      const value = JSON.parse(data) as RicaAuthenticationSession[];

      // Migration: If session doesn't have entraAccessToken, try to get it from accessToken
      // This handles old sessions created before entraAccessToken field was added
      const migratedSessions = value.map((session) => {
        if (!session.entraAccessToken && session.accessToken) {
          console.warn(
            "getSessions: Found old session without entraAccessToken. User should logout and login again for full functionality.",
          );
          // We can't reconstruct the Entra token from MDP token, so mark it as missing
          return {
            ...session,
            entraAccessToken: undefined, // Explicitly mark as missing
          };
        }
        return session;
      });

      return migratedSessions;
    } catch (e: any) {
      console.warn(`Error parsing sessions.json: ${e}`);
      return [];
    }
  }

  get onDidChangeSessions() {
    return this._sessionChangeEmitter.event;
  }

  get ideRedirectUri() {
    // We redirect to a page that says "you can close this page", and that page finishes the redirect
    const url = new URL("vscode://Rica.rica");
    return url.toString();
  }

  public static useOnboardingUri: boolean = false;
  get redirectUri() {
    if (EntraIDAuthProvider.useOnboardingUri) {
      const url = new URL("vscode://Rica.rica");
      return url.toString();
    }
    return this.ideRedirectUri;
  }

  async refreshSessions() {
    try {
      await this._refreshSessions();
    } catch (e) {
      console.error(`Error refreshing sessions: ${e}`);
    }
  }

  private async _refreshSessions(): Promise<void> {
    const sessions = await this.getSessions();
    if (!sessions.length) return;

    const finalSessions: RicaAuthenticationSession[] = [];
    for (const session of sessions) {
      try {
        const newSession = await this._refreshSession(session.refreshToken);
        finalSessions.push({
          ...session,
          accessToken: newSession.accessToken,
          refreshToken: newSession.refreshToken,
          expiresInMs: newSession.expiresInMs,
          entraAccessToken: newSession.entraAccessToken, // Update Entra token too
        });
      } catch (e: any) {
        console.debug(`Error refreshing session token: ${e.message}`);
        await this.debugAccessTokenValidity(
          session.accessToken,
          session.refreshToken,
        );
        this._sessionChangeEmitter.fire({
          added: [],
          removed: [session],
          changed: [],
        });
      }
    }

    if (finalSessions.length) {
      await this.storeSessions(finalSessions);
      this._sessionChangeEmitter.fire({
        added: [],
        removed: [],
        changed: finalSessions,
      });
      this._scheduleRefresh(finalSessions[0].expiresInMs);
    }
  }

  private async _refreshSession(refreshToken: string): Promise<{
    accessToken: string;
    refreshToken: string;
    expiresInMs: number;
    entraAccessToken: string;
  }> {
    const controlPlaneEnv = getControlPlaneEnvFromSettings();
    // Step 1: Refresh Entra token
    const response = await fetch(
      `${controlPlaneEnv.CONTROL_PLANE_URL}/oauth2/v2.0/token`,
      {
        method: "POST",
        headers: {
          "Content-Type": "application/x-www-form-urlencoded",
        },
        body: new URLSearchParams({
          client_id: controlPlaneEnv.ENTRA_ID_CLIENT_ID || "",
          redirect_uri: this.redirectUri,
          grant_type: "refresh_token",
          refresh_token: refreshToken,
        }),
      },
    );

    if (!response.ok) {
      const text = await response.text();
      let errorMessage = `HTTP ${response.status}: ${text}`;

      try {
        const errorData = JSON.parse(text);
        if (errorData.error === "invalid_grant") {
          errorMessage =
            "Refresh token has expired or been revoked. Please sign in again.";
        } else if (errorData.error_description) {
          errorMessage = `Auth error: ${errorData.error_description}`;
        }
      } catch {
        // If JSON parsing fails, use the original text
      }

      throw new Error(errorMessage);
    }

    const data = (await response.json()) as any;
    const freshEntraToken = data.access_token;
    const newRefreshToken = data.refresh_token;

    // Step 2: Exchange fresh Entra token with backend for new MDP token
    const { mdpToken, expiresInMs } =
      await this.exchangeTokenWithBackend(freshEntraToken);
    console.log(
      `[DEBUG] refresh session ${{
        accessToken: mdpToken, // Return MDP token
        refreshToken: newRefreshToken,
        expiresInMs: expiresInMs,
        entraAccessToken: freshEntraToken,
      }}`,
    );
    return {
      accessToken: mdpToken, // Return MDP token
      refreshToken: newRefreshToken,
      expiresInMs: expiresInMs,
      entraAccessToken: freshEntraToken, // Return fresh Entra token
    };
  }

  private _formatProfileLabel(
    firstName: string | null,
    lastName: string | null,
  ) {
    return ((firstName ?? "") + " " + (lastName ?? "")).trim();
  }

  /**
   * Create a new auth session
   * @param scopes
   * @returns
   */
  public async createSession(
    scopes: string[],
  ): Promise<RicaAuthenticationSession> {
    try {
      const controlPlaneEnv = getControlPlaneEnvFromSettings();
      const codeVerifier = generateRandomString(64);
      const codeChallenge = await generateCodeChallenge(codeVerifier);

      if (!isHubEnv(controlPlaneEnv)) {
        throw new Error("Login is disabled");
      }

      const token = await this.login(codeChallenge, controlPlaneEnv, scopes);
      if (!token) {
        throw new Error(`Continue login failure`);
      }

      const tokenInfo = (await this.getUserInfo(
        token,
        codeVerifier,
        controlPlaneEnv,
      )) as any;

      const { access_token, refresh_token } = tokenInfo;

      // Exchange Entra token with backend for MDP token
      const { mdpToken, expiresInMs } =
        await this.exchangeTokenWithBackend(access_token);

      // Get user info from the original Entra token (for profile)
      const full_name = this.getJwtField(access_token, "name") as string | null;
      const email = this.getJwtField(access_token, "preferred_username") as
        | string
        | null;

      if (!email) {
        throw new Error("No email found in the access token");
      }

      // Create session with MDP token instead of Entra token
      const session: RicaAuthenticationSession = {
        id: AUTH_NAME,
        accessToken: mdpToken, // 🔄 Use MDP token instead of Entra token
        refreshToken: refresh_token, // Keep original refresh token for renewal
        expiresInMs: expiresInMs, // Use MDP token expiry
        loginNeeded: false,
        entraAccessToken: access_token, // 🚀 NEW: Store original Entra token
        account: {
          label: full_name as string,
          id: email,
        },
        scopes: scopes,
      };

      await this.storeSessions([session]);

      this._sessionChangeEmitter.fire({
        added: [session],
        removed: [],
        changed: [],
      });

      // Schedule refresh based on MDP token expiry
      this._scheduleRefresh(expiresInMs);

      return session;
    } catch (e) {
      window.showErrorMessage(`Sign in failed: ${e}`);
      throw e;
    }
  }

  /**
   * Remove an existing session
   * @param sessionId
   */
  public async removeSession(sessionId: string): Promise<void> {
    const sessions = await this.getSessions();
    const sessionIdx = sessions.findIndex((s) => s.id === sessionId);
    const session = sessions[sessionIdx];
    sessions.splice(sessionIdx, 1);

    await this.storeSessions(sessions);

    if (session) {
      this._sessionChangeEmitter.fire({
        added: [],
        removed: [session],
        changed: [],
      });
    }
  }

  /**
   * Get the current Entra access token (not MDP token)
   * @returns Entra access token or undefined
   */
  public async getEntraAccessToken(): Promise<string | undefined> {
    const sessions = await this.getSessions();
    if (sessions.length === 0) {
      return undefined;
    }
    return sessions[0].entraAccessToken;
  }

  /**
   * Clear all stored sessions (useful when refresh tokens become invalid)
   */
  public async clearAllSessions(): Promise<void> {
    const sessions = await this.getSessions();
    if (sessions.length > 0) {
      await this.storeSessions([]);
      this._sessionChangeEmitter.fire({
        added: [],
        removed: sessions,
        changed: [],
      });
      console.log("All authentication sessions cleared");
    }
  }

  /**
   * Dispose the registered services
   */
  public async dispose() {
    // Clear any scheduled refreshes
    if (this._refreshIntervalId) {
      clearInterval(this._refreshIntervalId);
      this._refreshIntervalId = undefined;
    }
    if (this._refreshTimeoutId) {
      clearTimeout(this._refreshTimeoutId);
      this._refreshTimeoutId = undefined;
    }
    this._disposable.dispose();
  }

  /**
   * Log in to Continue
   */
  private async login(
    codeChallenge: string,
    // stateId: string,
    hubEnv: HubEnv,
    scopes: string[] = [],
  ) {
    return await window.withProgress<string>(
      {
        location: ProgressLocation.Notification,
        title: "Signing in to RICA...",
        cancellable: true,
      },
      async (_, token) => {
        const stateId = uuidv4();

        this._pendingStates.push(stateId);

        const controlPlaneEnv = getControlPlaneEnvFromSettings();
        // const scopeString = "2ff814a6-3304-4ab8-85cb-cd0e6f879c1d/.default";
        // const scopeString = `offline_access api://${controlPlaneEnv.ENTRA_ID_CLIENT_ID}/user_impersonation`;
        const scopeString = [
          "offline_access",
          `api://${controlPlaneEnv.ENTRA_ID_CLIENT_ID}/user_impersonation`,
        ].join(" ");
        const url = new URL(
          `${controlPlaneEnv.CONTROL_PLANE_URL}/oauth2/v2.0/authorize`,
        );
        const params = {
          response_type: "code",
          client_id: controlPlaneEnv.ENTRA_ID_CLIENT_ID || "",
          redirect_uri: "vscode://Rica.rica",
          // scope: "openid profile offline_access User.Read",
          scope: scopeString,
          code_challenge: codeChallenge,
          code_challenge_method: "S256",
          state: stateId,
        };

        Object.keys(params).forEach((key) =>
          url.searchParams.append(key, params[key as keyof typeof params]),
        );

        const oauthUrl = url;
        if (oauthUrl) {
          await env.openExternal(Uri.parse(oauthUrl.toString()));
        } else {
          return;
        }

        let codeExchangePromise = this._codeExchangePromises.get(scopeString);
        if (!codeExchangePromise) {
          codeExchangePromise = promiseFromEvent(
            this._uriHandler.event,
            this.handleUri(scopes),
          );
          this._codeExchangePromises.set(scopeString, codeExchangePromise);
        }

        try {
          return await Promise.race([
            codeExchangePromise.promise,
            new Promise<string>(
              (_, reject) =>
                setTimeout(() => reject("Cancelled"), 60 * 60 * 1_000), // 60min timeout
            ),
            promiseFromEvent<any, any>(
              token.onCancellationRequested,
              (_, __, reject) => {
                reject("User Cancelled");
              },
            ).promise,
          ]);
        } finally {
          this._pendingStates = this._pendingStates.filter(
            (n) => n !== stateId,
          );
          codeExchangePromise?.cancel.fire();
          this._codeExchangePromises.delete(scopeString);
        }
      },
    );
  }

  /**
   * Handle the redirect to VS Code (after sign in from Continue)
   * @param scopes
   * @returns
   */
  private handleUri: (
    scopes: readonly string[],
  ) => PromiseAdapter<Uri, string> =
    (scopes) => async (uri, resolve, reject) => {
      const query = new URLSearchParams(uri.query);
      const access_token = query.get("code");
      const state = query.get("state");

      if (!access_token) {
        reject(new Error("No token"));
        return;
      }
      if (!state) {
        reject(new Error("No state"));
        return;
      }

      // Check if it is a valid auth request started by the extension
      if (!this._pendingStates.some((n) => n === state)) {
        reject(new Error("State not found"));
        return;
      }

      resolve(access_token);
    };

  /**
   * Get the user info from WorkOS
   * @param token
   * @returns
   */
  private async getUserInfo(
    token: string,
    codeVerifier: string,
    hubEnv: HubEnv,
  ) {
    const controlPlaneEnv = getControlPlaneEnvFromSettings();
    const resp = await fetch(
      `${controlPlaneEnv.CONTROL_PLANE_URL}/oauth2/v2.0/token`,
      {
        method: "POST",
        headers: {
          "Content-Type": "application/x-www-form-urlencoded",
        },
        body: new URLSearchParams({
          client_id: controlPlaneEnv.ENTRA_ID_CLIENT_ID || "",
          redirect_uri: this.redirectUri,
          code_verifier: codeVerifier,
          grant_type: "authorization_code",
          code: token,
        }),
      },
    );
    const text = await resp.text();
    const data = JSON.parse(text);
    return data;
  }
}

export async function getControlPlaneSessionInfo(
  silent: boolean,
  useOnboarding: boolean,
): Promise<ControlPlaneSessionInfo | undefined> {
  try {
    if (useOnboarding) {
      EntraIDAuthProvider.useOnboardingUri = true;
    }

    // First try to get existing session
    let session = await authentication.getSession(
      AUTH_NAME,
      [],
      { silent: true }, // Always try silent first
    );

    // If we have a session, validate it's not expired
    if (session) {
      // Try to get the auth provider instance to validate token
      const providers = (authentication as any).providers;
      if (providers) {
        for (const [key, provider] of providers) {
          if (
            key === AUTH_NAME &&
            provider &&
            typeof provider.ensureValidToken === "function"
          ) {
            const isValid = await provider.ensureValidToken();
            if (!isValid) {
              // Token refresh failed, need new login
              console.log("Token validation failed, clearing session");
              session = undefined;
            }
            break;
          }
        }
      }
    }

    // If no valid session, create new one if not silent
    if (!session && !silent) {
      session = await authentication.getSession(AUTH_NAME, [], {
        createIfNone: true,
      });
    }

    if (!session) {
      return undefined;
    }

    EntraIDAuthProvider.useOnboardingUri = false;
    console.log({ session });

    const controlPlaneEnv = getControlPlaneEnvFromSettings();

    // Validate that we have a valid access token
    if (!session.accessToken) {
      console.error("[EntraIDAuthProvider] No access token in session", {
        sessionId: session.account.id,
        scopes: session.scopes,
      });
      throw new Error("No access token available in session");
    }

    console.log("[EntraIDAuthProvider] Returning session info with token", {
      hasToken: !!session.accessToken,
      tokenLength: session.accessToken?.length,
      tokenPrefix: session.accessToken?.substring(0, 20) + "...",
      accountId: session.account.id,
    });

    return {
      AUTH_TYPE: controlPlaneEnv.AUTH_TYPE,
      accessToken: session.accessToken,
      account: {
        id: session.account.id,
        label: session.account.label,
      },
    };
  } catch (e: any) {
    console.error(`Error getting control plane session info: ${e.message}`);
    if (!silent) {
      window.showErrorMessage(`Sign in failed: ${e.message}`);
    }
    return undefined;
  }
}

/**
 * Get Entra ID access token for backend API calls (not for LLM access)
 * This is separate from getControlPlaneSessionInfo which returns MDP token
 */
export async function getEntraIDAccessToken(
  silent: boolean = true,
): Promise<string | undefined> {
  try {
    const session = await authentication.getSession(
      AUTH_NAME,
      [],
      silent ? { silent: true } : { createIfNone: false },
    );
    if (!session) {
      console.warn("getEntraIDAccessToken: No session found");
      return undefined;
    }

    const ricaSession = session as RicaAuthenticationSession;

    if (!ricaSession.entraAccessToken) {
      console.warn(
        "getEntraIDAccessToken: Session found but entraAccessToken is missing. User needs to logout and login again.",
      );
      return undefined;
    }

    return ricaSession.entraAccessToken;
  } catch (e: any) {
    console.error(`Error getting Entra ID access token: ${e.message}`);
    return undefined;
  }
}
