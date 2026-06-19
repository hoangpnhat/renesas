/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import {
  CommonDialogProps,
  CommonDrawerProps,
  HistoryChatItem,
  MessageType,
} from "../typings/component.props";

export const FAKE_BOT_CHAT: MessageType[] = [];
export const INITIAL_DIALOG_HISTORY: HistoryChatItem[] = [];
export const DEFAULT_DIALOG_CONFIG: CommonDialogProps = {
  title: "Dialog",
  isOpen: false,
  children: <div>HelloWorld</div>,
  hasCloseButton: true,
  hasConfirmButton: true,
  confirmButtonText: "Confirm",
  cancelButtonText: "labelButton.cancel",
  hasCancelButton: true,
};
export const DEFAULT_DRAWER_CONFIG: CommonDrawerProps = {
  isOpen: false,
  title: "Drawer",
  confirmButtonText: "",
  cancelButtonText: "",
  hasCancelButton: false,
  drawerWidth: 300,
  anchor: "right",
  maxDrawerWidth: 600,
};
