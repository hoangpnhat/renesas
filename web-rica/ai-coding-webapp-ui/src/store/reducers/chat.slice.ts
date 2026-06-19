import { MessageStatus } from "./../../typings/component.props";
import { createSlice } from "@reduxjs/toolkit";
import type { PayloadAction } from "@reduxjs/toolkit";
import { MessageType } from "../../typings/component.props";
import { ChatContentStateReducer } from ".";

// Define a type for the slice state

// Define the initial state using that type
const initialState: ChatContentStateReducer = {
  data: [],
};

export const chatSlice = createSlice({
  name: "chat",
  // `createSlice` will infer the state type from the `initialState` argument
  initialState,
  reducers: {
    updateChatContent: (
      state,
      newItem: PayloadAction<Partial<ChatContentStateReducer>>,
    ) => {
      return {
        ...state,
        data: [...state.data, newItem.payload.data].flat() as MessageType[],
      };
    },
    newContent: (
      state,
      newItem: PayloadAction<MessageType | MessageType[]>,
    ) => {
      return {
        ...state,
        data: [newItem.payload, ...state.data].flat() as MessageType[],
      };
    },
    removeChatContent: (state, newItem: PayloadAction<MessageType>) => {
      return {
        ...state,
        data: state.data.filter(
          (message) => message.id !== (newItem.payload as MessageType).id,
        ),
      };
    },
    updateContentWithStatus: (
      state,
      newItem: PayloadAction<
        Partial<MessageType & { statusToUpdate: MessageStatus }>
      >,
    ) => {
      return {
        ...state,
        data: state.data.map((message) => {
          if (message.status === newItem.payload.statusToUpdate) {
            return { ...message, ...newItem.payload };
          }
          return message;
        }),
      };
    },
    removeErrorMessage: (
      state,
      newItem: PayloadAction<{ errorId: string }>,
    ) => {
      return {
        ...state,
        data: state.data.filter((message) => {
          return (
            message.id !== newItem.payload.errorId &&
            message.status !== "pending"
          );
        }),
      };
    },
    updateCompletedStatus: (state) => {
      return {
        ...state,
        data: state.data.map((message) => {
          if (message.status === "completed") {
            return { ...message, status: "" };
          }
          return message;
        }),
      };
    },

    updateContent: (state, newItem: PayloadAction<Partial<MessageType>>) => {
      return {
        ...state,
        data: state.data.map((message) => {
          if (message.id === (newItem.payload as MessageType).id) {
            return { ...message, ...newItem.payload };
          }
          return message;
        }),
      };
    },
    resetContent: () => {
      return initialState;
    },
    resetPage: (state) => {
      return { ...state, pageBreak: initialState.pageBreak };
    },
    updateAllNewContent: (
      _state,
      newItem: PayloadAction<Partial<ChatContentStateReducer>>,
    ) => {
      return newItem.payload as ChatContentStateReducer;
    },
  },
});

export const {
  updateChatContent,
  resetPage,
  newContent,
  removeChatContent,
  updateContentWithStatus,
  updateContent,
  resetContent,
  updateAllNewContent,
  removeErrorMessage,
  updateCompletedStatus,
} = chatSlice.actions;

export default chatSlice.reducer;
