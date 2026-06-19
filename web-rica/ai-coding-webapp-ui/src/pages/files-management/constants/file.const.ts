import { SelectionItem } from "../../../typings/component.props.ts";

export enum FileUploadActionValue {
  KEEP_BOTH = "keep_both",
  REPLACE = "replace",
}

export enum FileUploadActionName {
  KEEP_BOTH = "Keep both",
  REPLACE = "Replace",
}

export const FILE_REPLACE_ACTIONS: SelectionItem[] = [
  {
    name: FileUploadActionName.REPLACE,
    value: FileUploadActionValue.REPLACE,
  },
  {
    name: FileUploadActionName.KEEP_BOTH,
    value: FileUploadActionValue.KEEP_BOTH,
  },
];
