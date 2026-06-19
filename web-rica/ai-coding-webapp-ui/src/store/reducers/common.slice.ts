/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import type { PayloadAction } from "@reduxjs/toolkit";
import { createSlice } from "@reduxjs/toolkit";
import { CommonState } from "../../typings/common.props";

// Define a type for the slice state

// Define the initial state using that type
const initialState: CommonState = {
  isLoading: false,
  snackBar: { isOpen: false, message: "", type: "warning" },
  isSelectedSidebar: "",
  isThinkingChatBot: false,
  pageBreak: { page: 0, total_page: 1, total_records: 10 },
  language: "en",
  isShowingSideBar: false,
  toggleKey: undefined,
};

export const commonSlice = createSlice({
  name: "common",
  // `createSlice` will infer the state type from the `initialState` argument
  initialState,
  reducers: {
    updateCommon: (state, newItem: PayloadAction<Partial<CommonState>>) => {
      return { ...state, ...newItem.payload };
    },
    resetChatbotThinkingState: (state) => {
      return { ...state, isThinkingChatBot: false };
    },
    decreasePage: (state) => {
      return {
        ...state,
        pageBreak: {
          ...state.pageBreak,
          page: (state.pageBreak?.page as number) - 1,
        },
      };
    },
    increasePage: (state) => {
      return {
        ...state,
        pageBreak: {
          ...state.pageBreak,
          page: (state.pageBreak?.page as number) + 1,
        },
      };
    },
    closeSnackBar: (state) => {
      return { ...state, snackBar: { ...state.snackBar, isOpen: false } };
    },
  },
});

export const { closeSnackBar, updateCommon, resetChatbotThinkingState } =
  commonSlice.actions;

export default commonSlice.reducer;
