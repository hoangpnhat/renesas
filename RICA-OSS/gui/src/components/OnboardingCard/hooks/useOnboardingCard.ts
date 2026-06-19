import { OnboardingModes } from "core/protocol/core";
import { useNavigate } from "react-router-dom";
import { OnboardingCardState } from "..";
import { useAppDispatch, useAppSelector } from "../../../redux/hooks";
import {
  setDialogMessage,
  setOnboardingCard,
  setShowDialog,
} from "../../../redux/slices/uiSlice";
import { getLocalStorage, setLocalStorage } from "../../../util/localStorage";

export interface UseOnboardingCard {
  show: OnboardingCardState["show"];
  activeTab: OnboardingCardState["activeTab"];
  setActiveTab: (tab: OnboardingModes) => void;
  open: (tab?: OnboardingModes) => void;
  close: (isDialog?: boolean) => void;
}

export function useOnboardingCard(): UseOnboardingCard {
  const navigate = useNavigate();
  const dispatch = useAppDispatch();

  const onboardingCard = useAppSelector((state) => state.ui.onboardingCard);

  const onboardingStatus = getLocalStorage("onboardingStatus");
  const hasDismissedOnboardingCard = getLocalStorage(
    "hasDismissedOnboardingCard",
  );

  let show: boolean;

  // RICA: under EntraID the user is auto-configured by the Sang/Databricks
  // dynamic config, so the onboarding card (Credits / API-Key / Local provider
  // setup, Continue-branded logo) is functionally moot. Hide it everywhere via
  // this single chokepoint — it gates both the auto-open (Layout.tsx) and the
  // EmptyChatBody render (Chat.tsx). To re-enable, set the flag to false.
  // (Phase 5.5 branding.)
  const RICA_HIDE_ONBOARDING_CARD = true;

  // Always show if we explicitly want to, e.g. passing free trial
  // and setting up keys
  if (RICA_HIDE_ONBOARDING_CARD) {
    show = false;
  } else if (onboardingCard.show) {
    show = true;
  } else {
    show = onboardingStatus !== "Completed" && !hasDismissedOnboardingCard;
  }

  async function open(tab?: OnboardingModes) {
    navigate("/");
    dispatch(
      setOnboardingCard({
        show: true,
        activeTab: tab ?? OnboardingModes.MODELS_ADD_ON,
      }),
    );
  }

  function close(isDialog = false) {
    setLocalStorage("hasDismissedOnboardingCard", true);
    dispatch(setOnboardingCard({ show: false }));
    if (isDialog) {
      dispatch(setDialogMessage(undefined));
      dispatch(setShowDialog(false));
    }
  }

  function setActiveTab(tab: OnboardingModes) {
    dispatch(setOnboardingCard({ show: true, activeTab: tab }));
  }

  return {
    show,
    setActiveTab,
    open,
    close,
    activeTab: onboardingCard.activeTab,
  };
}
