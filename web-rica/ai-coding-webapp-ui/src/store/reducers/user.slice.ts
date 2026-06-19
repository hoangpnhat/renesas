/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { AvatarCardProps } from "./../../typings/component.props";
import { createSlice, PayloadAction } from "@reduxjs/toolkit";

// Define the initial state using that type
const initialState: AvatarCardProps = {
  name: "",
  department: "",
  email: "",
  roles: [],
  avatarUrl: ""
};

export const chatSlice = createSlice({
  name: "user",
  initialState,
  reducers: {
    updateUserDetails: (
      state,
      action: PayloadAction<Partial<AvatarCardProps>>
    ) => {
      // Chỉ update những field thực sự có trong payload
      const payload = action.payload;

      if (payload.name !== undefined) {
        state.name = payload.name;
      }
      if (payload.department !== undefined) {
        state.department = payload.department;
      }
      if (payload.email !== undefined) {
        state.email = payload.email;
      }
      if (payload.roles !== undefined) {
        state.roles = payload.roles;
      }
      if (payload.avatarUrl !== undefined) {
        state.avatarUrl = payload.avatarUrl;
      }
    }
  }
});

export const { updateUserDetails } = chatSlice.actions;

export default chatSlice.reducer;
