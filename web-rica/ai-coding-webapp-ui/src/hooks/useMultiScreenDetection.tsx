/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { useEffect, useMemo, useState } from "react";
import { WIDTH_THRESHOLD } from "../constants/common.ts";

interface ScreenInfo {
  id: string;
  isPrimary: boolean;
  width: number;
  height: number;
  availWidth: number;
  availHeight: number;
  left: number;
  top: number;
}

interface MultiScreenData {
  isSupported: boolean;
  screens: ScreenInfo[];
  currentScreen: ScreenInfo | null;
  isExtendedMode: boolean;
  totalScreenWidth: number;
  totalScreenHeight: number;
  error?: string;
}

declare global {
  interface Window {
    getScreens?: () => Promise<ScreenInfo[]>;
  }

  interface Screen {
    isExtended?: boolean;
    availLeft?: number;
    availTop?: number;
  }
}

/**
 * Custom hook to track multi-screen information if supported
 * Uses Window Management API (Chrome 100+, Edge 100+)
 *
 * @returns {MultiScreenData} Multi-screen state information
 */
const useMultiScreenDetection = (
  threshold: number = WIDTH_THRESHOLD,
): {
  multiScreenData: MultiScreenData;
  isLaptopScreen: boolean;
  currentScreen: ScreenInfo;
} => {
  const [multiScreenData, setMultiScreenData] = useState<MultiScreenData>({
    isSupported: false,
    screens: [],
    currentScreen: null,
    isExtendedMode: false,
    totalScreenWidth: 0,
    totalScreenHeight: 0,
  });

  const isLaptopScreen = useMemo(() => {
    return multiScreenData.currentScreen
      ? multiScreenData?.currentScreen?.width <= threshold
      : false;
  }, [multiScreenData.currentScreen, threshold]);

  useEffect(() => {
    const isWindowManagementSupported: boolean =
      "getScreens" in window ||
      ("screen" in window && "isExtended" in window.screen);

    setMultiScreenData((prev) => ({
      ...prev,
      isSupported: isWindowManagementSupported,
    }));

    if (!isWindowManagementSupported) {
      return;
    }

    const detectScreens = async (): Promise<void> => {
      try {
        let screens: ScreenInfo[] = [];

        if ("getScreens" in window && window.getScreens) {
          screens = await window.getScreens();
        } else if ("screen" in window && "isExtended" in window.screen) {
          const isExtended = window.screen.isExtended;

          if (isExtended) {
            screens = [
              {
                id: "primary",
                isPrimary: true,
                width: window.screen.width,
                height: window.screen.height,
                availWidth: window.screen.availWidth,
                availHeight: window.screen.availHeight,
                left: 0,
                top: 0,
              },
            ];

            if (
              window.screen.availLeft !== undefined ||
              window.screen.availTop !== undefined
            ) {
              screens.push({
                id: "secondary",
                isPrimary: false,
                width: window.screen.width,
                height: window.screen.height,
                availWidth: window.screen.availWidth,
                availHeight: window.screen.availHeight,
                left: window.screen.availLeft || 0,
                top: window.screen.availTop || 0,
              });
            }
          } else {
            screens = [
              {
                id: "single",
                isPrimary: true,
                width: window.screen.width,
                height: window.screen.height,
                availWidth: window.screen.availWidth,
                availHeight: window.screen.availHeight,
                left: 0,
                top: 0,
              },
            ];
          }
        }

        let totalWidth: number = 0;
        let maxHeight: number = 0;

        screens.forEach((screen) => {
          totalWidth += screen.width;
          if (screen.height > maxHeight) maxHeight = screen.height;
        });

        let currentScreen: ScreenInfo | null = null;
        if (screens.length > 0) {
          const windowX: number = window.screenX || window.screenLeft;
          const windowY: number = window.screenY || window.screenTop;

          currentScreen =
            screens.find((screen) => {
              return (
                windowX >= screen.left &&
                windowX < screen.left + screen.width &&
                windowY >= screen.top &&
                windowY < screen.top + screen.height
              );
            }) || screens[0];
        }

        setMultiScreenData({
          isSupported: true,
          screens,
          currentScreen,
          isExtendedMode: screens.length > 1,
          totalScreenWidth: totalWidth,
          totalScreenHeight: maxHeight,
        });
      } catch (error) {
        console.error("Error accessing screen information:", error);

        setMultiScreenData((prev) => ({
          ...prev,
          error: error instanceof Error ? error.message : String(error),
        }));
      }
    };

    detectScreens();

    const handleScreenChange = (): void => {
      detectScreens();
    };

    window.addEventListener("resize", handleScreenChange);

    if (
      "screen" in window &&
      "orientation" in window.screen &&
      window.screen.orientation
    ) {
      window.screen.orientation.addEventListener("change", handleScreenChange);
    }

    return () => {
      window.removeEventListener("resize", handleScreenChange);
      if (
        "screen" in window &&
        "orientation" in window.screen &&
        window.screen.orientation
      ) {
        window.screen.orientation.removeEventListener(
          "change",
          handleScreenChange,
        );
      }
    };
  }, []);

  return {
    multiScreenData,
    isLaptopScreen,
    currentScreen: multiScreenData.currentScreen as ScreenInfo,
  };
};

export default useMultiScreenDetection;
