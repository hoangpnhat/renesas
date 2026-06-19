import { createSlice } from "@reduxjs/toolkit";
import type { PayloadAction } from "@reduxjs/toolkit";
import { InputContextType } from "../../typings/context.dto";
import { CommonInterface } from "../../components/CommonFormGroup.tsx";

// Define a type for the slice state

// Define the initial state using that type
const initialState: InputContextType = {
  inputText: "",
  isLoading: false,
  vector_search: [],
};

export const inputSlice = createSlice({
  name: "input",
  // `createSlice` will infer the state type from the `initialState` argument
  initialState,
  reducers: {
    updateText(state, text: PayloadAction<string>) {
      return {
        ...state,
        inputText: text.payload,
      };
    },
    updateInputContext(
      state,
      newState: PayloadAction<Partial<InputContextType>>,
    ) {
      return { ...state, ...newState.payload };
    },
    updateStatus(state, status: PayloadAction<boolean>) {
      return { ...state, isLoading: status.payload };
    },
    removeInput(state) {
      return { ...state, inputText: "" };
    },
    updateUserOption(state, action: PayloadAction<CommonInterface>) {
      const newState = action.payload;
      const vector_search: string[] = Object.keys(newState).reduce(
        (previousValue, currentValue) => {
          if (newState[currentValue]) {
            return [...previousValue, currentValue];
          }
          return previousValue;
        },
        [] as string[],
      );
      return { ...state, vector_search };
    },
  },
});

export const {
  updateText,
  updateStatus,
  removeInput,
  updateInputContext,
  updateUserOption,
} = inputSlice.actions;

export default inputSlice.reducer;
