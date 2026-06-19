/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import { SideBarItem } from "../typings/component.props";
import { NewChatIcon } from "../components/Icons";
import AttachFileOutlinedIcon from "@mui/icons-material/AttachFileOutlined";
import DocumentScannerOutlinedIcon from "@mui/icons-material/DocumentScannerOutlined";
import FormatListBulletedOutlinedIcon from "@mui/icons-material/FormatListBulletedOutlined";
import { APP_ROLE } from "../constants/common.ts";
import { ToolInternalEnums } from "../pages/tool-management/data/userToolsData.tsx";

export const sidebarOptionItems: SideBarItem[] = [
  {
    to: "management",
    key: "sideBarItem.management",
    name: "sideBarItem.management",
    toggleKey: "management",
    icon: <FormatListBulletedOutlinedIcon />,
    disabled: false,
    roles: [APP_ROLE.USER, APP_ROLE.ADMIN],
    toolNames: [ToolInternalEnums.CREATION, ToolInternalEnums.RULE_CHECK],
    children: [
      {
        to: "management/files",
        key: "sideBarItem.fileManagement",
        name: "sideBarItem.fileManagement",
        icon: <AttachFileOutlinedIcon />,
        disabled: false,
        roles: [APP_ROLE.ADMIN, APP_ROLE.USER],
        toolNames: [ToolInternalEnums.CREATION]
      },
      {
        to: "management/knowledge",
        key: "sideBarItem.knowledgeBaseManagement",
        name: "sideBarItem.knowledgeBaseManagement",
        icon: <DocumentScannerOutlinedIcon />,
        disabled: false,
        roles: [APP_ROLE.ADMIN, APP_ROLE.USER],
        toolNames: [ToolInternalEnums.CREATION]
      }
    ]
  },
  {
    to: "chat/undefined",
    key: "new_chat",
    name: "newChat",
    icon: <NewChatIcon />,
    disabled: false,
  },
  // Additional items here
];
