/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { createSlice } from "@reduxjs/toolkit";

export interface MessageError {
  name: string;
  message: string;
}

export interface ErrorsStateReducer {
  [key: string]: MessageError;
}

export const initialState: ErrorsStateReducer = {};
export const errorsSliceName = "errors";
export const errorsSlice = createSlice({
  name: errorsSliceName,
  // `createSlice` will infer the state type from the `initialState` argument
  initialState,
  reducers: {
    addErrorMessage: (state, newItem: { payload: MessageError }) => {
      return {
        ...state,
        [newItem.payload.name]: newItem.payload,
      };
    },
    removeErrorMessage: (state, newItem: { payload: { errorId: string } }) => {
      const newState = { ...state };
      if (
        Object.prototype.hasOwnProperty.call(newState, newItem.payload.errorId)
      ) {
        delete newState[newItem.payload.errorId];
      }
      return newState;
    },
    resetErrorMessages: () => {
      return { ...initialState };
    },
    updateErrorMessage: (
      state,
      newItem: { payload: Partial<MessageError> & { name: string } },
    ) => {
      if (state?.[newItem.payload.name]) {
        return {
          ...state,
          [newItem.payload.name]: {
            ...state[newItem.payload.name],
            ...newItem.payload,
          },
        };
      } else {
        return {
          ...state,
          [newItem.payload.name]: {
            name: newItem.payload.name,
            message: newItem.payload.message || "",
          },
        };
      }
    },
  },
});
export const { removeErrorMessage, updateErrorMessage, resetErrorMessages } =
  errorsSlice.actions;
export default errorsSlice.reducer;
