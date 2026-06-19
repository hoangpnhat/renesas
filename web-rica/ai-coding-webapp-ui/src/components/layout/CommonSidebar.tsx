/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import Divider from "@mui/material/Divider";
import Drawer from "@mui/material/Drawer";
import useTheme from "@mui/material/styles/useTheme";
import React, { PropsWithChildren, useRef } from "react";

import { useNavigate } from "react-router-dom";
import {
  CommonSidebarProps,
  SideBarItem,
} from "../../typings/component.props.ts";
import classNames from "classnames";

import { useSelector } from "react-redux";
import { RootState } from "../../store";
import AvatarCard from "../auth/AvatarCard.tsx";
import { CommonLanguageSwitcher } from "../CommonLanguageSwitcher.tsx";
import { languageSelection } from "../../models/components.model.tsx";
import RICALogo from "/image/RICA_icon_medium.png";
import { CommonNestedMenuItem } from "./CommonNestedMenuItem.tsx";

export const Sidebar = ({
  children,
  sidebarItems,
  isOpen,
  drawerWidth = 275,
}: PropsWithChildren<CommonSidebarProps>) => {
  const navigate = useNavigate();
  const commonContext = useSelector((state: RootState) => state.common);
  const sidebarRef = useRef<HTMLDivElement>(null);

  const theme = useTheme();

  const handleSideBarOption = (
    event: React.MouseEvent<HTMLDivElement>,
    item: SideBarItem,
  ) => {
    event.preventDefault();
    navigate(item.to, { state: { customKey: item.name, toPath: item.to } });
  };
  return (
    <React.Fragment>
      <Drawer
        className="box-border"
        sx={{
          flexShrink: 0,
          width: isOpen ? drawerWidth : 0,
          height: "100%",
          "& .MuiDrawer-paper": {
            width: drawerWidth,
            boxSizing: "border-box",
            overflow: "hidden",
            opacity: 1,
            backgroundColor: theme.palette.background.default,
            display: "flex",
            flexDirection: "column",
            justifyContent: "space-between",
          },
        }}
        open={isOpen}
        variant="persistent"
      >
        <nav className="h-full flex flex-col bg-[inherit]">
          <header className="w-full flex justify-between items-center space-x-3 rtl:space-x-reverse my-2">
            <a href="" title="Renesas Logo" className="flex">
              <img alt="RICA Logo" className="!w-[128px]" src={RICALogo} />
            </a>
            <CommonLanguageSwitcher selectionItems={languageSelection} />
          </header>

          {/* Main content area with flex-grow to take available space */}
          <div className="flex-1 flex flex-col min-h-0">
            <ul className="space-y-2 font-medium w-full">
              {sidebarItems.map((item) => (
                <CommonNestedMenuItem
                  key={item.key}
                  item={item}
                  level={0}
                  selectedKey={commonContext.isSelectedSidebar}
                  onSelect={handleSideBarOption}
                  isActive={item.isActive ?? false}
                />
              ))}
            </ul>
            <Divider />
            <div
              id="default-sidebar"
              ref={sidebarRef}
              className={classNames(
                "overflow-x-hidden overflow-y-auto flex-1 min-w-inherit flex transition-transform -translate-x-full sm:translate-x-0",
                `w-[${drawerWidth}px] relative`,
              )}
              aria-label="Sidebar"
              onMouseDown={(e) => e.preventDefault()}
            >
              <div className="h-full w-full px-3 my-2">{children}</div>
              <div className="app-sidebar-resizer flex-grow-0 flex-shrink-0 justify-self-end cursor-col-resize resize-x hover:w-[5px] basis-1 hover:bg-[#c1c3c5b4]" />
            </div>
          </div>

          {/* Footer area with AvatarCard */}
          <div className="mt-auto">
            <AvatarCard />
          </div>
        </nav>
      </Drawer>
    </React.Fragment>
  );
};
