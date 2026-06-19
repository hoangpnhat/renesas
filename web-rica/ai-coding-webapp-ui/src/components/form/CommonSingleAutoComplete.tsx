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
  autoCompleteOptions?: MultipleAutoComplete<T>;
  onChange: (value: T | null) => void;
  error?: boolean;
  value: T | null;
  helperText?: string;
  textFieldProps: TextFieldProps;
}

export const CommonSingleAutoComplete = forwardRef(
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
        <Autocomplete<T, false, false, false, "div">
          options={datum}
          value={value ?? null}
          onChange={(_, newValue) => onChange(newValue ?? null)}
          renderInput={(params) => (
            <TextField
              {...params}
              {...textFieldProps}
              error={error}
              helperText={helperText}
              inputRef={ref}
            />
          )}
          {...(autoCompleteOptions as Partial<
            AutocompleteProps<T, false, false, false, "div">
          >)}
        />
      </Fragment>
    );
  },
) as <T>(
  props: CommonAutoCompleteProps<T> & { ref?: React.Ref<HTMLInputElement> },
) => JSX.Element;
