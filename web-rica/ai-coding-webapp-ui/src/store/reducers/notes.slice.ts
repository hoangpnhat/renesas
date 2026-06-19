import { createSlice, PayloadAction } from "@reduxjs/toolkit";
// import { NoteType } from "../../typings/component.props";
import { SavedChatContentStateReducer } from ".";

// Define a type for the slice state

// Define the initial state using that type
const initialState: SavedChatContentStateReducer = {
  pageBreak: {
    page: 0,
    total_page: 0,
    total_records: 0,
  },
  data: [],
};

export const noteSlice = createSlice({
  name: "notes",
  // `createSlice` will infer the state type from the `initialState` argument
  initialState,
  reducers: {
    updateAllNewNote(
      _state,
      newValue: PayloadAction<SavedChatContentStateReducer>
    ) {
      return newValue.payload;
    },
  },
});

export const { updateAllNewNote } = noteSlice.actions;

export default noteSlice.reducer;
