import { RULE_NAME_DISPLAY } from "../constants/common";
import { HistoryChatItem } from "./component.props";

export interface BaseInterface {
  id: string;
}

export interface ModelComponent<T> {
  id: keyof T;
  displayName: string;
  render: (value: string | string[]) => JSX.Element;
}

export interface BaseActionMenu<T extends string | HistoryChatItem> {
  [K: string]: (model: T) => void | Promise<void>;
}

export interface BaseObject {
  [K: string]: CommonType;
}

export type BaseObjectWithInterface<T, V = string> = {
  [K in keyof T]: V;
};

export type CommonType = string | number | boolean | string[];

export type RULE_NAME_TYPE = keyof typeof RULE_NAME_DISPLAY;
