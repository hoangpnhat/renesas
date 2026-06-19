import type { PayloadAction } from "@reduxjs/toolkit";
import { createSlice } from "@reduxjs/toolkit";
import { HistoryChatItem } from "../../typings/component.props";
import { removeMultiple } from "../../utils/utilities.ts";

// Define a type for the slice state

// Define the initial state using that type
const initialState: HistoryChatItem[] = [];

export const dialogSlice = createSlice({
  name: "dialog",
  // `createSlice` will infer the state type from the `initialState` argument
  initialState,
  reducers: {
    removeDialogItem: (
      state,
      item: PayloadAction<Partial<HistoryChatItem>>,
    ) => {
      return state.filter((val) => val.id !== item.payload.id);
    },
    removeDialogItems: (state, listItems: PayloadAction<string[]>) => {
      return removeMultiple<HistoryChatItem>(state, listItems.payload, "id");
    },
    updateDialogWithNewItem: (
      state,
      newItem: PayloadAction<HistoryChatItem>,
    ) => {
      return [
        newItem.payload,
        ...state.filter((item) => {
          return newItem.payload.id !== item.id;
        }),
      ];
    },
    updateNew: (_state, item: PayloadAction<HistoryChatItem[]>) => {
      return item.payload;
    },
    addDialog: (
      state,
      item: PayloadAction<HistoryChatItem | HistoryChatItem[]>,
    ) => {
      return Array.isArray(item)
        ? [...state, ...(item.payload as HistoryChatItem[])].flat()
        : [...state, item.payload as HistoryChatItem];
    },
    addLatestDialog: (
      state,
      item: PayloadAction<HistoryChatItem | HistoryChatItem[]>,
    ) => {
      return Array.isArray(item)
        ? [...(item.payload as HistoryChatItem[]), ...state].flat()
        : [item.payload as HistoryChatItem, ...state];
    },
    // eslint-disable-next-line @typescript-eslint/no-unused-vars
    resetDialog: (_state) => {
      return [] as HistoryChatItem[];
    },
  },
});

export const {
  removeDialogItem,
  updateNew,
  addDialog,
  resetDialog,
  updateDialogWithNewItem,
  addLatestDialog,
  removeDialogItems,
} = dialogSlice.actions;

export default dialogSlice.reducer;
