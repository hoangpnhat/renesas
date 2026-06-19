import { createSlice, PayloadAction } from "@reduxjs/toolkit";

export type ProgressStatus = "fulfilled" | "error" | "pending"

export interface ProgressStateProps {
  loadingPercent: number;
  status: ProgressStatus;
  isLoading: boolean;
}

export interface ProgressState {
  [documentId: string]: ProgressStateProps;
}

const initialState: ProgressState = {};

export const progressSlice = createSlice({
  name: "progress",
  initialState,
  reducers: {
    updateProgress(state, action: PayloadAction<Partial<ProgressStateProps> & { documentId: string }>) {
      const { documentId, ...progressInfo } = action.payload;
      state[documentId] = {
        ...state[documentId],
        ...progressInfo
      };
    },
    // eslint-disable-next-line @typescript-eslint/no-unused-vars
    resetProgress(_state) {
      return initialState;
    }
  }
});

export const {
  updateProgress,
  resetProgress
} = progressSlice.actions;

export default progressSlice.reducer;