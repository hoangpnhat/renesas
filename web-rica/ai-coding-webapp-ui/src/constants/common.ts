/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

export const ENTRIES_STANDARD_RECORDS = 10;
export const TEXTAREA_TEXT_LENGTH_LIMIT = 1000;
export const ENTRIES_LARGE_RECORDS = 100;

export enum ROLE {
  USER = "user",
  ASSISTANT = "assistant",
}

export enum HTTP_METHOD {
  GET = "get",
  POST = "post",
  PATCH = "patch",
  DELETE = "delete",
  PUT = "PUT",
}

export enum FileTypeReader {
  STRING = "string",
  ARRAY_BUFFER = "array",
  JSON = "json",
}

export enum BUTTON_VALUE_NAME {
  ADD_NEW_DIALOG = "add_new_dialog",
  NOTES = "notes",
}

export enum RAG_NAME_VALUE {
  "code_base" = "code_base",
  "document" = "document",
}

export enum RAG_NAME_DISPLAY {
  "code_base" = "Code",
  "ringi_basic_vector_search" = "Ringi",
  "document" = "Document",
}

export enum COMMON_STATUS {
  PROCESSING = "processing",
  SUCCESS = "success",
  FAILED = "failed",
  CANCELLED = "cancelled",
  DONE = "done",
}

export enum APP_ROLE {
  ADMIN = "Admin",
  READ_PERMISSION = "read",
  USER = "User",
  SUPER_ADMIN = "Super_Admin",
}

export const ENTRIES_PER_PAGE_SMALL = 6;
export const ENTRIES_PER_PAGE = 10;
export const WIDTH_THRESHOLD = 1300;

export enum HeaderAbbreviationHeaderValue {
  TERM = "term",
  DESCRIPTION = "description",
}

export const REQUIRED_HEADERS = [
  HeaderAbbreviationHeaderValue.TERM.valueOf(),
  HeaderAbbreviationHeaderValue.DESCRIPTION.valueOf(),
];

export enum Placement {
  Right = "right",
  Bottom = "bottom",
}

export const fileStatus = {
  processing: {
    label: "Processing",
    bgcolor: "rgba(251, 191, 36, 0.15)" /* warning-main tint */,
    color: "#FBBF24" /* warning-main */,
    icon: "⏳",
    borderColor: "rgba(251, 191, 36, 0.4)",
  },
  completed: {
    label: "Completed",
    bgcolor: "rgba(16, 185, 129, 0.15)" /* success-main tint */,
    color: "#10B981" /* success-main */,
    icon: "✓",
    borderColor: "rgba(16, 185, 129, 0.4)",
  },
  failed: {
    label: "Failed",
    bgcolor: "rgba(248, 113, 113, 0.15)" /* error-main tint */,
    color: "#F87171" /* error-main */,
    icon: "✕",
    borderColor: "rgba(248, 113, 113, 0.4)",
  },
  default: {
    label: "Default",
    bgcolor: "rgba(91, 89, 211, 0.15)" /* primary-main tint */,
    color: "#7875E8" /* primary-light */,
    icon: "◉",
    borderColor: "rgba(91, 89, 211, 0.4)",
  },
};
