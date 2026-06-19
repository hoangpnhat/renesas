/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

export const END_POINT = {
  dialog: "chats/dialog",
  download: "chats/download-file",
  dialogs: "chats/dialogs",
  saved_content: "chats/saved_contents",
  user: "user",
  userMe: "user/me",
  userSearch: "user/search",
  userSettings: "user/me/settings",
  admin: {
    modelManagement: {
      base: "admin/model-management",
      modelDatabricks: "admin/model-management/model-databricks",
      modelRoles: "admin/model-management/model-roles",
      configuration: "admin/model-management/configs",
    },
  },
  file: "file",
  knowledge: "knowledge",
  knowledgeDefault: "knowledge/default",
  fileUpload: "file/upload",
  group: "group",
  notifications: "notification",
  customize: {
    prompt: "prompt",
  },
};
