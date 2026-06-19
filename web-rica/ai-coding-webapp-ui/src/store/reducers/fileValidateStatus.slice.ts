import { createSlice, PayloadAction } from "@reduxjs/toolkit";

export type ValidationStatus = "passed" | "failed";

export interface FileValidateStatusProps {
  status: ValidationStatus;
}

export interface FileValidateStatusState {
  [documentId: string]: FileValidateStatusProps;
}

const initialState: FileValidateStatusState = {};

export const fileValidateStatusSlice = createSlice({
  name: "fileValidateStatus",
  initialState,
  reducers: {
    updateValidation: (
      state,
      action: PayloadAction<
        Partial<FileValidateStatusProps> & { documentId: string }
      >,
    ) => {
      const { documentId, ...validateInfo } = action.payload;

      state[documentId] = {
        ...state[documentId],
        ...validateInfo,
      };
    },
    removeValidationWithDocumentId: (
      state,
      action: PayloadAction<{ documentId: string }>,
    ) => {
      const { documentId } = action.payload;

      delete state[documentId];
    },
    // eslint-disable-next-line @typescript-eslint/no-unused-vars
    resetValidation: (_state) => {
      return initialState as FileValidateStatusState;
    },
  },
});

export const {
  updateValidation,
  removeValidationWithDocumentId,
  resetValidation,
} = fileValidateStatusSlice.actions;

export default fileValidateStatusSlice.reducer;
