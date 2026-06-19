/**
 * RICA VS Code Auth Integration Layer
 *
 * This module provides the boundary between upstream VS Code extension
 * and RICA-specific authentication (EntraID).
 *
 * Per RICA_REPO_STANDARD.md:
 * - Auth boundary: all auth goes through rica/adapters/auth/
 * - Extension boundary: mapping layer between upstream and RICA
 * - Modified-upstream: minimal patches to extension
 */

import * as vscode from "vscode";
import {
  ControlPlaneSessionInfo,
  AuthType,
} from "../../../rica/adapters/auth/AuthTypes";
import {
  EntraIDAuthProvider,
  getControlPlaneSessionInfo,
} from "../../../rica/adapters/auth/EntraIDAuthProvider";
import { UriEventHandler } from "../../../extensions/vscode/src/stubs/uriHandler";

/**
 * RICA Auth Provider - wraps EntraID authentication
 * Replaces WorkOsAuthProvider from upstream Continue
 */
export class RicaAuthProvider {
  private entraIdProvider: EntraIDAuthProvider;

  constructor(context: vscode.ExtensionContext, uriHandler: UriEventHandler) {
    this.entraIdProvider = new EntraIDAuthProvider(context, uriHandler);
  }

  /**
   * Refresh sessions (called on activation)
   */
  async refreshSessions(): Promise<void> {
    return await this.entraIdProvider.refreshSessions();
  }

  /**
   * Dispose auth provider
   */
  dispose(): void {
    this.entraIdProvider.dispose();
  }

  /**
   * Get underlying EntraID provider for advanced usage
   */
  getEntraIdProvider(): EntraIDAuthProvider {
    return this.entraIdProvider;
  }
}

/**
 * Get control plane session info for RICA
 * This function is called by VsCodeMessenger when Core requests session info
 *
 * Re-exported from EntraIDAuthProvider with same signature as upstream WorkOS version
 * for minimal patching.
 */
export { getControlPlaneSessionInfo as getRicaControlPlaneSessionInfo };
