/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import React, { Suspense, useEffect, useState } from "react";
import { LayoutHeaderComponent } from "./Header";
import type { AppBarProps } from "@mui/material/AppBar";
import { Outlet } from "react-router-dom";
import { CommonSnackBar } from "../components/CommonSnackBar";
import { CommonDialogComponent } from "../components/dialog/CommonDialog.tsx";

import { HistoryChatItem } from "../typings/component.props";
import classNames from "classnames";
import { CommonDrawerComponent } from "../components/Drawer";
import { useSelector } from "react-redux";
import { RootState } from "../store";

import LinearIndeterminate from "../components/CommonProgressBar";
import { CommonLoading } from "../components/CommonLoading";
import { useChangePath } from "../hooks/useChangePath";
import { useTranslation } from "react-i18next";
import { getUserMe } from "../pages/requests/user-request.ts";
import { AxiosResponse } from "axios";
import { GlobalSocketProvider } from "../hooks/useGlobalSocket.tsx";

import { CommonMenuWithIcon } from "../components/CommonMenuWithIcon.tsx";
import { SkeletonHistoryChat } from "../components/skeletons/SkeletonHistoryChat.tsx";
import { Sidebar } from "../components/layout/CommonSidebar.tsx";
import { UserMeResponseBody } from "../typings/request.ts";
import { useSidebarOptions } from "./useSidebarOptions.tsx";

const HistoryChat = React.lazy(
  () => import("../pages/chat/components/HistoryChat"),
);

export interface MainLayoutProps {
  header: AppBarProps;
  hasDrawer: boolean;
}

export const MainLayoutComponent = () => {
  const loadingContext = useSelector(
    (state: RootState) => state.common.isLoading,
  );

  const sidebarOptionItemsState = useSidebarOptions();
  const { i18n } = useTranslation();
  useEffect(() => {
    getUserMe().then(
      (response: AxiosResponse<UserMeResponseBody | null>) => {
        if (response.data?.settings?.preferred_language) {
          i18n.changeLanguage(response.data?.settings?.preferred_language);
          return;
        }
        i18n.changeLanguage("en");
      },
    );

    // Listen for language changes

    // Clean up the listener on component unmount
  }, []);
  const [isPin] = useState(true);

  const dialogContext = useSelector((state: RootState) => state.dialog);
  useChangePath();
  return (
    <GlobalSocketProvider>
      <div className="h-full overflow-hidden edu-au-vic-wa-nt-guides">
        {/* <CommonLoading isLoading={loadingContext as boolean} /> */}
        <CommonDialogComponent />
        <CommonMenuWithIcon />
        <CommonSnackBar
          autoHideDuration={4000}
          anchorOrigin={{ vertical: "top", horizontal: "right" }}
        />

        <div className="z-[1000]">
          <LinearIndeterminate isLoading={loadingContext as boolean} />
        </div>
        {!isPin && <LayoutHeaderComponent />}
        <div
          className={classNames("relative z-0 flex w-full overflow-hidden", {
            "h-full": isPin,
            "h-[95%]": !isPin,
          })}
        >
          <div className="relative flex h-full max-w-full flex-1 flex-col overflow-hidden">
            <div className="flex w-full h-full mt-2">
              <Sidebar
                drawerWidth={250}
                isOpen={true}
                sidebarItems={sidebarOptionItemsState}
              >
                {location.pathname.includes("chat") && (
                  <Suspense fallback={<SkeletonHistoryChat />}>
                    <HistoryChat history={dialogContext as HistoryChatItem[]} />
                  </Suspense>
                )}
              </Sidebar>
              <main
                className={classNames(
                  "relative h-full flex-1 transition-width max-w-[calc(97%-225px)]",
                )}
              >
                <div className="flex h-full flex-col focus-visible:outline-0 w-full">
                  <Suspense fallback={<CommonLoading />}>
                    <Outlet />
                  </Suspense>
                </div>
              </main>
              <CommonDrawerComponent />
            </div>
          </div>
        </div>
      </div>
    </GlobalSocketProvider>
  );
};
