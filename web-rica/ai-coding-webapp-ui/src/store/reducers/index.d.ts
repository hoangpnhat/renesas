import { MessageType } from "../../typings/component.props";
import { SavedChatContent } from "../../typings/request";

declare module "@mui/material/styles" {
  interface Palette {
    sidebar: Palette["primary"];
    knowledge: Palette["primary"];
    abbreviate: Palette["primary"];
  }

  interface PaletteOptions {
    sidebar?: PaletteOptions["primary"];
    knowledge?: PaletteOptions["primary"];
    abbreviate?: PaletteOptions["primary"];
  }
}

export interface PageBreak {
  page: number;
  total_page: number;
  total_records: number;
}

export interface BaseData<T> {
  pageBreak?: PageBreak;
  data: T;
}
export interface SavedChatContentStateReducer
  extends BaseData<SavedChatContent[]> {}
export interface ChatContentStateReducer extends BaseData<MessageType[]> {}
