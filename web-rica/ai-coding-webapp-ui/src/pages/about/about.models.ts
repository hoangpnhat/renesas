import { handleActionAboutUs } from "../../utils/storeDispatch";
import type { DisplayColumnType } from "../chat/typings/state";

export const listColumnNames: string[] = ["ruleName", "category", "actions"];
export const displayColumns = {
  ruleName: "Rules Name",
  category: "Category",
  actions: "Actions",
} as const;
export const itemRows: DisplayColumnType[] = [
  {
    ruleName: "Renesas Electronics Group CSR Charter",
    category: "Management & Governance",
    actions: <T = string>(item: T) => {
      handleActionAboutUs<T>(item);
    },
  },
  {
    ruleName: "Basic Rules of the Internal Control Promotion Committee",
    category: "Management & Governance",
    actions: <T = string>(item: T) => {
      handleActionAboutUs<T>(item);
    },
  },
  {
    ruleName: "Renesas Electronics Group Executive Committee rules",
    category: "Management & Governance",
    actions: <T = string>(item: T) => {
      handleActionAboutUs<T>(item);
    },
  },
  {
    ruleName:
      "Basic Rules for Management and Operation of Related Companies, etc.",
    category: "Management & Governance",
    actions: <T = string>(item: T) => {
      handleActionAboutUs<T>(item);
    },
  },
];
