import type { PayloadAction } from "@reduxjs/toolkit";
import { createSlice } from "@reduxjs/toolkit";
import { CommonDrawerProps } from "../../typings/component.props";

// Define a type for the slice state

// Define the initial state using that type
const initialState: CommonDrawerProps = {
  drawerWidth: 0,
  anchor: "right",
  maxDrawerWidth: 0,
  isOpen: false,
  title: "",
  confirmButtonText: "",
  cancelButtonText: "",
  hasCancelButton: false,
  keepMounted: true,
  isOverflow: true,
};

export const drawerSlice = createSlice({
  name: "drawer",
  // `createSlice` will infer the state type from the `initialState` argument
  initialState,
  reducers: {
    closeDrawer(state) {
      return { ...state, isOpen: false };
    },
    openDrawer(state) {
      return { ...state, isOpen: true };
    },
    updateDrawerConfig(
      state,
      newDialogConfig: PayloadAction<Partial<CommonDrawerProps>>,
    ) {
      return { ...state, ...newDialogConfig.payload };
    },
  },
});

export const { closeDrawer, openDrawer, updateDrawerConfig } =
  drawerSlice.actions;

export default drawerSlice.reducer;
