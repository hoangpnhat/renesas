/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import {
  Control,
  Controller,
  FieldErrors,
  FieldValues,
  Path,
} from "react-hook-form";
import InputAdornment from "@mui/material/InputAdornment";
import TextField from "@mui/material/TextField";
import type { TextFieldProps } from "@mui/material/TextField/TextField";
import { FormItem } from "../../typings/common.props.ts";
import { CommonButtonWithIcon } from "../button/CommonButtonWithIcon.tsx";
import { PlayArrowOutlined } from "@mui/icons-material";
import { useCallback } from "react";
import { ErrorsStateReducer } from "../../store/reducers/errors.slice.ts";

export interface RunApiProps {
  onClickRunApi: () => void;
  isTesting?: boolean;
  isDisabled?: boolean;
  errors?: ErrorsStateReducer;
  runApiTitle?: string;
}

interface CommonInputFieldProps<T, TType extends FieldValues> {
  item: FormItem<T>;
  control: Control<TType>;
  errors: FieldErrors<TType>;
  index: number;
  actionType?: "normal" | "run-api";
  runApi?: RunApiProps;
  onChangeValue?: (newValue: string) => void;
}

export const CommonInputField = <T, TType extends FieldValues>({
  item,
  control,
  errors,
  index,
  runApi,

  onChangeValue,
  actionType = "normal",
}: CommonInputFieldProps<T, TType>) => {
  const onRunApi = useCallback(async () => {
    runApi?.onClickRunApi?.();
  }, [runApi]);

  return (
    <Controller
      control={control}
      key={index}
      render={({ field }) => {
        return (
          <TextField
            {...(item.properties as TextFieldProps)}
            {...field}
            value={field.value || ""}
            onChange={(event) => {
              const newValue = event.target.value;

              field.onChange(event); // vẫn giữ để RHF sync
              onChangeValue?.(newValue);
            }}
            error={
              // !!runApi?.errors?.[item.name] ||
              !!errors[item.name as keyof TType]
            }
            helperText={
              errors[item.name as keyof TType]?.message as React.ReactNode
            }
            margin="normal"
            className="w-full"
            sx={(theme) => ({
              "& .MuiOutlinedInput-root": {
                borderRadius: "12px",
                backgroundColor: theme.palette.background.paper,
                transition: "all 0.2s ease-in-out",
                "& fieldset": {
                  borderColor: theme.palette.divider,
                },
                "&:hover": {
                  backgroundColor: theme.palette.action.hover,
                  transform: "translateY(-1px)",
                  boxShadow: "0 4px 12px rgba(0, 0, 0, 0.3)",
                  "& fieldset": {
                    borderColor: theme.palette.text.secondary,
                  },
                },
                "&.Mui-focused": {
                  backgroundColor: theme.palette.background.paper,
                  "& fieldset": {
                    borderColor: theme.palette.primary.main,
                    borderWidth: "2px",
                  },
                },
                "&.Mui-error": {
                  "& fieldset": {
                    borderColor: theme.palette.error.main,
                  },
                  "&:hover fieldset": {
                    borderColor: theme.palette.error.dark,
                  },
                },
              },
              "& .MuiInputLabel-root": {
                fontWeight: 500,
                "&.Mui-focused": {
                  color: theme.palette.primary.main,
                },
                "&.Mui-error": {
                  color: theme.palette.error.main,
                },
              },
              "& .MuiFormHelperText-root": {
                marginLeft: 1,
                fontSize: "0.875rem",
                "&.Mui-error": {
                  color: theme.palette.error.main,
                  fontWeight: 500,
                },
              },
            })}
            {...(actionType === "run-api"
              ? {
                  InputProps: {
                    endAdornment: (
                      <InputAdornment position="end">
                        <CommonButtonWithIcon
                          title={runApi?.runApiTitle || "Test Connection"}
                          disabled={!field.value}
                          onClick={onRunApi}
                        >
                          <PlayArrowOutlined />
                        </CommonButtonWithIcon>
                      </InputAdornment>
                    ),
                  },
                }
              : {})}
          />
        );
      }}
      name={item.name as Path<TType>}
    />
  );
};
