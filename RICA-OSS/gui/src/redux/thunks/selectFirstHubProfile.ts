import { createAsyncThunk } from "@reduxjs/toolkit";
import { ThunkApiType } from "../store";

/**
 * Phase 5.1.26 (Group 5 — WorkOS auth severed): on Continue, this thunk
 * selected the first `profileType === "platform"` profile after onboarding
 * — i.e. the just-created Hub assistant — and posted
 * `didChangeSelectedProfile` to swap to it. RICA never produces a
 * "platform" profile (Hub APIs are severed in Group 4 and the EntraID
 * flow surfaces only `EntraIDDynamicConfig` profiles), so the original
 * lookup would always poll three times then resolve `void` anyway.
 *
 * Two onboarding components still dispatch this thunk after a successful
 * `auth.login(true)`:
 *   - gui/src/components/OnboardingCard/components/OnboardingCardLanding.tsx
 *   - gui/src/components/OnboardingCard/components/OnboardingModelsAddOnTab.tsx
 *
 * The OnboardingCard flow itself is queued for a Phase 5.4.5 disposition
 * (rebrand / rewire to EntraID-via-Sang / hide entirely). Until that
 * decision lands, this thunk needs to exist to satisfy the imports and
 * resolve cleanly so the chained `.then(...)` callbacks (showTutorial,
 * showToast, onboardingCard.close) still run. Imports + signature are
 * preserved so the OnboardingCard component code does not need to be
 * touched in Group 5; the body is a no-op.
 */
export const selectFirstHubProfile = createAsyncThunk<
  void,
  undefined,
  ThunkApiType
>("selectFirstHubProfile", async () => {
  // Intentional no-op for the Rica build — no platform profiles exist.
  return;
});
