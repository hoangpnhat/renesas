/**
 * RICA CLI PostHog Shim
 *
 * Disables all PostHog telemetry. RICA does not send telemetry to Continue's
 * PostHog instance. This stub replaces the upstream posthogService with no-ops.
 */

class PosthogServiceStub {
  uniqueId = "rica-cli";

  async capture(
    _event: string,
    _properties?: Record<string, any>,
  ): Promise<void> {}
  async shutdown(): Promise<void> {}
  async identify(
    _userId: string,
    _properties?: Record<string, any>,
  ): Promise<void> {}
}

export const posthogService = new PosthogServiceStub();
