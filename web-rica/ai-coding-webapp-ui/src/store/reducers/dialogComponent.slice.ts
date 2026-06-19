/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import type { PayloadAction } from "@reduxjs/toolkit";
import { createSlice } from "@reduxjs/toolkit";
import { CommonDialogProps, OmitPartialCommonDialogProps } from "../../typings/component.props";

// Define a type for the slice state

// Define the initial state using that type
const initialState: CommonDialogProps = {
  isOpen: false,
  title: "",
  confirmButtonText: "labelButton.confirm",
  cancelButtonText: "labelButton.cancel",
  hasCancelButton: true,
  hasConfirmButton: true,
  hasAction: true,
  maxWidth: "lg",
  fullWidth: false
};

export const dialogComponentSlice = createSlice({
  name: "dialogComponent",
  // `createSlice` will infer the state type from the `initialState` argument
  initialState,
  reducers: {
    closeDialog(state) {
      return {
        ...state,
        isOpen: false,
        typeButton: "button",
        handleOnConfirm: undefined,
        formNameSubmitButton: undefined,
        onClose: undefined,
        children: undefined,
        onSubmit: undefined
      };
    },
    pushDialog(state, action: PayloadAction<OmitPartialCommonDialogProps>) {
      const newDialog = action.payload;
      return {
        ...state,
        isOpen: true,
        componentStacks: [...(state.componentStacks || []), newDialog]
      };
    },
    popDialog(state) {
      state?.componentStacks?.pop();
      if (state?.componentStacks?.length === 0) {
        state.isOpen = false;
        state.typeButton = "button";
        state.handleOnConfirm = undefined;
        state.formNameSubmitButton = undefined;
        state.onClose = undefined;
        state.onSubmit = undefined;
      }
    },

    updateDialogConfig(
      state,
      newDialogConfig: PayloadAction<Partial<CommonDialogProps>>
    ) {
      return { ...state, ...newDialogConfig.payload };
    }
  }
});

export const { closeDialog, updateDialogConfig, popDialog, pushDialog } =
  dialogComponentSlice.actions;

export default dialogComponentSlice.reducer;
