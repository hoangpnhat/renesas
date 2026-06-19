import { createSlice, PayloadAction } from "@reduxjs/toolkit";
import { Notification } from "../../typings/request";

export interface NotificationState {
  unreadCount: number;
  notifications: Notification[];
}

const initialState: NotificationState = {
  unreadCount: 0,
  notifications: [],
};

export const notificationSlice = createSlice({
  name: "notification",
  initialState,
  reducers: {
    setNotifications: (state, action: PayloadAction<Notification[]>) => {
      state.notifications = action.payload;
      state.unreadCount = action.payload.filter(
        (n) => n.status === "unread",
      ).length;
    },
    incrementUnreadCount: (state) => {
      state.unreadCount += 1;
    },
    markAsRead: (state, action: PayloadAction<string>) => {
      const notification = state.notifications.find(
        (n) => n.id === action.payload,
      );
      if (notification && notification.status === "unread") {
        notification.status = "read";
        state.unreadCount = Math.max(0, state.unreadCount - 1);
      }
    },
    resetNotifications: (state) => {
      state.notifications = [];
      state.unreadCount = 0;
    },
  },
});

export const {
  setNotifications,
  incrementUnreadCount,
  markAsRead,
  resetNotifications,
} = notificationSlice.actions;

export default notificationSlice.reducer;
