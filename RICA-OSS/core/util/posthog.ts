import os from "node:os";

import { TeamAnalytics } from "../control-plane/TeamAnalytics.js";
import { IdeInfo } from "../index.js";

import type { PostHog as PostHogType } from "posthog-node";
import { extractMinimalStackTraceInfo } from "./extractMinimalStackTraceInfo.js";
import { TokensBatchingService } from "./TokensBatchingService.js";

export enum PosthogFeatureFlag {
  AutocompleteTimeout = "autocomplete-timeout",
  RecentlyVisitedRangesNumSurroundingLines = "recently-visited-ranges-num-surrounding-lines",
}

export const EXPERIMENTS: {
  [key in PosthogFeatureFlag]: {
    [key: string]: { value: any };
  };
} = {
  [PosthogFeatureFlag.AutocompleteTimeout]: {
    control: { value: 150 },
    "250": { value: 250 },
    "350": { value: 350 },
    "450": { value: 450 },
  },
  [PosthogFeatureFlag.RecentlyVisitedRangesNumSurroundingLines]: {
    control: { value: null },
    "5": { value: 5 },
    "10": { value: 10 },
    "15": { value: 15 },
    "20": { value: 20 },
  },
};

export class Telemetry {
  // Set to undefined whenever telemetry is disabled
  static client: PostHogType | undefined = undefined;
  static uniqueId = "NOT_UNIQUE";
  static os: string | undefined = undefined;
  static ideInfo: IdeInfo | undefined = undefined;

  /**
   * Convenience method for capturing errors in a single event
   */
  static async captureError(errorName: string, error: unknown) {
    if (!(error instanceof Error)) {
      return;
    }
    await Telemetry.capture(
      "extension_error_caught",
      {
        errorName,
        message: error.message,
        stack: extractMinimalStackTraceInfo(error.stack),
      },
      false,
    );
  }

  static async capture(
    event: string,
    properties: { [key: string]: any },
    sendToTeam: boolean = false,
    isExtensionActivationError: boolean = false,
  ) {
    try {
      const augmentedProperties = {
        ...properties,
        os: Telemetry.os,
        extensionVersion: Telemetry.ideInfo?.extensionVersion,
        ideName: Telemetry.ideInfo?.name,
        ideType: Telemetry.ideInfo?.ideType,
      };
      const payload = {
        distinctId: Telemetry.uniqueId,
        event,
        properties: augmentedProperties,
        sendFeatureFlags: true,
      };

      // In cases where an extremely early fatal error occurs, we may not have initialized yet
      if (isExtensionActivationError && !Telemetry.client) {
        const client = await Telemetry.getTelemetryClient();
        client?.capture(payload);
        return;
      }

      if (process.env.NODE_ENV === "test") {
        return;
      }

      Telemetry.client?.capture(payload);

      if (sendToTeam) {
        void TeamAnalytics.capture(event, properties);
      }
    } catch (e) {
      console.error(`Failed to capture event: ${e}`);
    }
  }

  static shutdownPosthogClient() {
    TokensBatchingService.getInstance().shutdown();
    Telemetry.client?.shutdown();
  }

  static async getTelemetryClient(): Promise<PostHogType | undefined> {
    // RICA (telemetry severed): RICA never instantiates
    // the Continue PostHog client. Returning undefined keeps Telemetry.client
    // permanently null, which makes `Telemetry.client?.capture(...)` and
    // `Telemetry.client?.getFeatureFlag(...)` throughout this file no-op.
    // The public surface (Telemetry.capture, captureError, getFeatureFlag)
    // stays intact so the dozens of callsites across core/extensions don't
    // need to change. continue.dev / app.posthog.com hardcode preserved
    // below as a paper trail of what was severed; unreachable.
    //
    //   const { PostHog } = await import("posthog-node");
    //   return new PostHog("phc_JS6XFROuNbhJtVCEdTSYk6gl5ArRrTNMpCcguAXlSPs", {
    //     host: "https://app.posthog.com",
    //   });
    return undefined;
  }

  static async setup(allow: boolean, uniqueId: string, ideInfo: IdeInfo) {
    Telemetry.uniqueId = uniqueId;
    Telemetry.os = os.platform();
    Telemetry.ideInfo = ideInfo;

    // RICA: client always remains undefined for the RICA build —
    // getTelemetryClient is the no-op above. Keep the original gate shape
    // for parity with upstream so any future re-enable just needs to flip
    // getTelemetryClient.
    if (!allow || process.env.NODE_ENV === "test") {
      Telemetry.client = undefined;
    } else if (!Telemetry.client) {
      Telemetry.client = await Telemetry.getTelemetryClient();
    }
  }

  private static featureValueCache: Record<string, any> = {};

  static async getFeatureFlag(flag: PosthogFeatureFlag) {
    const value = Telemetry.client?.getFeatureFlag(flag, Telemetry.uniqueId);

    Telemetry.featureValueCache[flag] = value;
    return value;
  }

  static async getValueForFeatureFlag(flag: PosthogFeatureFlag) {
    try {
      if (Telemetry.featureValueCache[flag]) {
        return Telemetry.featureValueCache[flag];
      }

      const userGroup = await Telemetry.getFeatureFlag(flag);
      if (typeof userGroup === "string") {
        return EXPERIMENTS[flag][userGroup].value;
      }

      return undefined;
    } catch {
      return undefined;
    }
  }
}
