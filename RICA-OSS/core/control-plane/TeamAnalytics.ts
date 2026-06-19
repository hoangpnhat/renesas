import os from "node:os";

import ContinueProxyAnalyticsProvider from "./analytics/ContinueProxyAnalyticsProvider.js";
import {
  ControlPlaneProxyInfo,
  IAnalyticsProvider,
} from "./analytics/IAnalyticsProvider.js";
import LogStashAnalyticsProvider from "./analytics/LogStashAnalyticsProvider.js";
import PostHogAnalyticsProvider from "./analytics/PostHogAnalyticsProvider.js";
import { ControlPlaneClient } from "./client.js";
import { AnalyticsConfig } from "../index.js";

function createAnalyticsProvider(
  config: AnalyticsConfig,
): IAnalyticsProvider | undefined {
  // RICA (telemetry severed): never instantiate any of
  // the upstream analytics providers in the RICA build. The "continue-proxy"
  // case POSTed events to api.continue.dev/proxy/analytics; "posthog"
  // POSTed to app.posthog.com; "logstash" hit a Continue-managed Logstash
  // endpoint. None are valid for Renesas. Returning undefined here makes
  // TeamAnalytics.provider permanently undefined, so .capture(...) elsewhere
  // is a no-op (already gated behind ?.).
  // doLoadConfig.ts already has TeamAnalytics.setup() commented out as of
  // upstream — this is defense-in-depth for any future re-enable.
  void config;
  return undefined;
  /*
  // @ts-ignore
  switch (config.provider) {
    case "posthog":
      return new PostHogAnalyticsProvider();
    case "logstash":
      return new LogStashAnalyticsProvider();
    case "continue-proxy":
      return new ContinueProxyAnalyticsProvider();
    default:
      return undefined;
  }
  */
}

export class TeamAnalytics {
  static provider: IAnalyticsProvider | undefined = undefined;
  static uniqueId = "NOT_UNIQUE";
  static os: string | undefined = undefined;
  static extensionVersion: string | undefined = undefined;

  static async capture(event: string, properties: { [key: string]: any }) {
    void TeamAnalytics.provider?.capture(event, {
      ...properties,
      os: TeamAnalytics.os,
      extensionVersion: TeamAnalytics.extensionVersion,
    });
  }

  static async setup(
    config: AnalyticsConfig,
    uniqueId: string,
    extensionVersion: string,
    controlPlaneClient: ControlPlaneClient,
    controlPlaneProxyInfo: ControlPlaneProxyInfo,
  ) {
    TeamAnalytics.uniqueId = uniqueId;
    TeamAnalytics.os = os.platform();
    TeamAnalytics.extensionVersion = extensionVersion;

    TeamAnalytics.provider = createAnalyticsProvider(config);
    await TeamAnalytics.provider?.setup(
      config,
      uniqueId,
      controlPlaneProxyInfo,
    );

    if (config.provider === "continue-proxy") {
      (
        TeamAnalytics.provider as ContinueProxyAnalyticsProvider
      ).controlPlaneClient = controlPlaneClient;
    }
  }

  static async shutdown() {
    if (TeamAnalytics.provider) {
      await TeamAnalytics.provider.shutdown();
      TeamAnalytics.provider = undefined;
      TeamAnalytics.os = undefined;
      TeamAnalytics.extensionVersion = undefined;
      TeamAnalytics.uniqueId = "NOT_UNIQUE";
    }
  }
}
