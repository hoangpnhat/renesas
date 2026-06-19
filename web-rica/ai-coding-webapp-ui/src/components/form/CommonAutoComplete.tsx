import { forwardRef, Fragment } from "react";
import {
  Autocomplete,
  AutocompleteProps,
  TextField,
  TextFieldProps,
} from "@mui/material";
import { MultipleAutoComplete } from "../../typings/common.props.ts";

interface CommonAutoCompleteProps<T> {
  datum: T[];
  autoCompleteOptions?: MultipleAutoComplete<T>; // Ensure T is properly typed
  onChange: (value: T[]) => void;
  error?: boolean;
  value: T[];
  helperText?: string;
  textFieldProps: TextFieldProps;
}

// Use forwardRef correctly
export const CommonAutoComplete = forwardRef(
  <T,>(
    {
      datum,
      onChange,
      autoCompleteOptions,
      error,
      value,
      textFieldProps,
      helperText,
    }: CommonAutoCompleteProps<T>,
    ref: React.Ref<HTMLInputElement>,
  ) => {
    return (
      <Fragment>
        <Autocomplete<T, true, true, false, "div"> // ✅ Explicitly type T
          options={datum}
          value={value ?? []} // Ensure value is never undefined
          onChange={(_, newValue) => onChange(newValue ?? [])} // Handle null case
          renderInput={(params) => (
            <TextField
              {...params}
              {...textFieldProps}
              error={error}
              helperText={helperText}
              inputRef={ref} // Pass the ref to the input field
            />
          )}
          {...(autoCompleteOptions as Partial<
            AutocompleteProps<T, true, true, false, "div">
          >)} // ✅ Ensure correct type casting
        />
      </Fragment>
    );
  },
) as <T>(
  props: CommonAutoCompleteProps<T> & { ref?: React.Ref<HTMLInputElement> },
) => JSX.Element; // ✅ Fix type inference issue
