/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import React, { useCallback } from "react";
import {
  InputAdornment,
  TextField,
  TextFieldProps,
  Typography,
} from "@mui/material";
import { Control, Controller, FieldValues, Path } from "react-hook-form";
import { TEXTAREA_TEXT_LENGTH_LIMIT } from "../../constants/common.ts";

export interface MaxCharsInputProps<TFieldValues extends FieldValues> {
  name: Path<TFieldValues>;
  maxChars: number;
  control: Control<TFieldValues>;
  textFieldProps?: Omit<TextFieldProps, "onChange" | "value">;
  error?: boolean;
  helperText?: React.ReactNode;
}

export const MaxCharsInput = <TFieldValues extends FieldValues>({
  name,
  maxChars = 100,
  control,
  textFieldProps,
  error,
  helperText,
}: MaxCharsInputProps<TFieldValues>) => {
  const getCounterColor = useCallback(
    (length: number) => {
      if (length >= maxChars) return "error.main";
      if (length >= maxChars * 0.9) return "warning.main";
      return "text.secondary";
    },
    [maxChars],
  );

  return (
    <Controller
      name={name}
      control={control}
      render={({ field }) => {
        const currentLength = field.value?.toString().length || 0;

        return (
          <TextField
            {...field}
            {...textFieldProps}
            sx={{
              width: "100%",
              "& textarea": {
                width:
                  currentLength > TEXTAREA_TEXT_LENGTH_LIMIT
                    ? "1200px"
                    : "100%",
              },
            }}
            value={field.value}
            slotProps={{
              htmlInput: {
                maxLength: maxChars,
                ...textFieldProps?.slotProps?.htmlInput,
              },
              input: {
                endAdornment: (
                  <InputAdornment position="end">
                    <Typography
                      variant="caption"
                      color={getCounterColor(currentLength)}
                    >
                      {currentLength}/{maxChars}
                    </Typography>
                  </InputAdornment>
                ),
                ...textFieldProps?.slotProps?.input,
              },
            }}
            error={error}
            helperText={helperText}
            margin="normal"
            onChange={(e) => {
              const newValue = e.target.value;
              if (newValue.length <= maxChars) {
                field.onChange(e);
              }
            }}
          />
        );
      }}
    />
  );
};
