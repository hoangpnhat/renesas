import { JSONContent } from "@tiptap/react";
import { OnboardingStatus } from "../components/OnboardingCard";

type LocalStorageTypes = {
  isExploreDialogOpen: boolean;
  hasDismissedExploreDialog: boolean;
  onboardingStatus?: OnboardingStatus;
  hasDismissedOnboardingCard: boolean;
  mainTextEntryCounter: number;
  ide: "vscode" | "jetbrains";
  vsCodeUriScheme: string;
  fontSize: number;
  [key: `inputHistory_${string}`]: JSONContent[];
  extensionVersion: string;
  showTutorialCard: boolean;
  shownProfilesIntroduction: boolean;
  disableIndexing: boolean;
  hasExitedFreeTrial: boolean;
  hasDismissedCliInstallBanner: boolean;
  // Multi-agent UX gating (RICA v1.2.1):
  //   - hasSeenMultiAgentNudge: legacy v1, kept for migration; suppressed once v2 fires
  //   - hasSeenMultiAgentNudge_v2: post-v1.1.6 redesigned nudge
  //   - hasSeenMultiAgentTour_v1: 3-step guided tour completion (EN/JP)
  hasSeenMultiAgentNudge: boolean;
  hasSeenMultiAgentNudge_v2: boolean;
  hasSeenMultiAgentTour_v1: boolean;
};

export enum LocalStorageKey {
  IsExploreDialogOpen = "isExploreDialogOpen",
  HasDismissedExploreDialog = "hasDismissedExploreDialog",
  HasExitedFreeTrial = "hasExitedFreeTrial",
}

export function getLocalStorage<T extends keyof LocalStorageTypes>(
  key: T,
): LocalStorageTypes[T] | undefined {
  const value = localStorage.getItem(key);

  if (value === null) {
    return undefined;
  }

  try {
    return JSON.parse(value);
  } catch (error) {
    console.error(
      `Error parsing ${key} from local storage. Value was ${value}\n\n`,
      error,
    );
    return undefined;
  }
}

export function setLocalStorage<T extends keyof LocalStorageTypes>(
  key: T,
  value: LocalStorageTypes[T],
): void {
  localStorage.setItem(key, JSON.stringify(value));

  // Dispatch custom event to notify current tab listeners
  window.dispatchEvent(
    new CustomEvent("localStorageChange", {
      detail: { key, value },
    }),
  );
}
