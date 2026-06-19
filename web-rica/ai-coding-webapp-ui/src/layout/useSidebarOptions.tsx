/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { SideBarItem } from "../typings/component.props";

import { useMemo } from "react";
import { APP_ROLE } from "../constants/common.ts";
import { useSelector } from "react-redux";
import { RootState } from "../store";

import SupervisorAccountIcon from "@mui/icons-material/SupervisorAccount";
import { DataIcon } from "../components/icons/DataIcon.tsx";
// import { NewChatIcon } from "../components/icons/NewChatIcon.tsx";
import { checkIfKeyInArr } from "../utils/utilities.ts";
import { ToolInternalEnums } from "../pages/tool-management/data/userToolsData.tsx";
import AttachFileOutlinedIcon from "@mui/icons-material/AttachFileOutlined";
import FormatListBulletedOutlinedIcon from "@mui/icons-material/FormatListBulletedOutlined";
import DocumentScannerOutlinedIcon from "@mui/icons-material/DocumentScannerOutlined";
import GroupIcon from "@mui/icons-material/Group";
import TuneIcon from "@mui/icons-material/Tune";
import GavelIcon from "@mui/icons-material/Gavel";
import AutoAwesomeIcon from "@mui/icons-material/AutoAwesome";

const sidebarOptionItems: SideBarItem[] = [
  {
    to: "admin",
    key: "admin",
    name: "title.layout.admin",
    icon: <SupervisorAccountIcon />,
    disabled: false,
    roles: [APP_ROLE.SUPER_ADMIN, APP_ROLE.ADMIN],
    children: [
      {
        to: "admin/model-management",
        key: "dataControl",
        name: "title.layout.modelManagement",
        icon: <DataIcon />,
        disabled: false,
        roles: [APP_ROLE.SUPER_ADMIN, APP_ROLE.ADMIN],
      },
    ],
  },
  {
    to: "management",
    key: "sideBarItem.management",
    name: "sideBarItem.management",
    toggleKey: "management",
    icon: <FormatListBulletedOutlinedIcon />,
    disabled: false,
    roles: [APP_ROLE.USER, APP_ROLE.ADMIN],
    toolNames: [ToolInternalEnums.CREATION, ToolInternalEnums.RULE_CHECK],
    isActive: true,
    children: [
      {
        to: "management/knowledgeBase",
        key: "knowledgeBase",
        name: "title.layout.knowledgeBase",
        icon: <DocumentScannerOutlinedIcon />,
        disabled: false,
        roles: [APP_ROLE.ADMIN, APP_ROLE.USER],
        toolNames: [ToolInternalEnums.CREATION],
      },
      {
        to: "management/file",
        key: "file",
        name: "title.layout.file",
        icon: <AttachFileOutlinedIcon />,
        disabled: false,
        roles: [APP_ROLE.ADMIN, APP_ROLE.USER],
        toolNames: [ToolInternalEnums.CREATION],
      },
      {
        to: "management/groups",
        key: "groups",
        name: "title.layout.groups",
        icon: <GroupIcon />,
        disabled: false,
        roles: [APP_ROLE.SUPER_ADMIN, APP_ROLE.ADMIN, APP_ROLE.USER],
      },
    ],
  },
  {
    to: "customize",
    key: "customize",
    name: "title.layout.customize",
    toggleKey: "customize",
    icon: <TuneIcon />,
    disabled: false,
    roles: [APP_ROLE.SUPER_ADMIN, APP_ROLE.ADMIN, APP_ROLE.USER],
    isActive: true,
    children: [
      {
        to: "customize/rule",
        key: "rule",
        name: "customize.typeRule",
        icon: <GavelIcon />,
        disabled: false,
        roles: [APP_ROLE.SUPER_ADMIN, APP_ROLE.ADMIN, APP_ROLE.USER],
      },
      {
        to: "customize/prompt",
        key: "prompt",
        name: "customize.typePrompt",
        icon: <AutoAwesomeIcon />,
        disabled: false,
        roles: [APP_ROLE.SUPER_ADMIN, APP_ROLE.ADMIN, APP_ROLE.USER],
      },
    ],
  },

  // {
  //   to: "chat/undefined",
  //   key: "new_chat",
  //   name: "title.layout.newChat",
  //   icon: <NewChatIcon />,
  //   disabled: false,
  //   roles: [APP_ROLE.SUPER_ADMIN, APP_ROLE.ADMIN, APP_ROLE.USER],
  // },
];

// Update `sidebarOptionItems` once translations are fully loaded
export const useSidebarOptions = () => {
  const userRoles = useSelector((state: RootState) => state.user.roles);
  return useMemo(() => {
    switch (true) {
      case checkIfKeyInArr(userRoles, APP_ROLE.ADMIN):
        return sidebarOptionItems.filter((item) =>
          checkIfKeyInArr(item.roles, APP_ROLE.ADMIN),
        );
      case checkIfKeyInArr(userRoles, APP_ROLE.USER):
        return sidebarOptionItems.filter((item) =>
          checkIfKeyInArr(item.roles, APP_ROLE.USER),
        );
      default:
        return sidebarOptionItems;
    }
  }, [userRoles]);
};
