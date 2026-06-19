import { SelectionItem } from "../../../typings/component.props";
import { displayColumns } from "../../about/about.models";

export type DisplayColumKeys = keyof typeof displayColumns;
export type DisplayColumnType = {
  [key in Exclude<DisplayColumKeys, "actions">]: string;
} & {
  actions: <T>(item: T) => void;
};
export interface UserSelectionState {
  categories: SelectionItem[];
  vector_search: SelectionItem;
}
