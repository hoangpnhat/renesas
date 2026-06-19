/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { Control, Controller, FieldPath } from "react-hook-form";

import type { TextFieldProps } from "@mui/material/TextField";
import TextField from "@mui/material/TextField";
import { useIconGetter } from "../../hooks/useIconGetter.tsx";
import { useTranslation } from "react-i18next";

interface FileDisplayFieldProps<T extends Record<string, any>> {
  control: Control<T>;
  name: FieldPath<T>;
  label: string;
  key?: number;
  properties?: Partial<TextFieldProps>;
}

export const CommonTextDisplay = <T extends Record<string, any>>({
  control,
  name,
  label,

  properties,
}: FileDisplayFieldProps<T>) => {
  const { getIconFileType } = useIconGetter({
    fontSize: "small",
    className: "m-2",
  });
  const { t } = useTranslation();

  return (
    <Controller
      control={control}
      render={({ field }) => {
        return (
          <TextField
            {...(properties as TextFieldProps)}
            value={field.value}
            label={`${label} (${t("text.topic.label.viewOnly")})`}
            variant="outlined"
            margin="normal"
            slotProps={{
              input: {
                readOnly: true,
                startAdornment: getIconFileType(field.value, false),
              },
            }}
            fullWidth
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
              },
              "& .MuiInputLabel-root": {
                fontWeight: 500,
                "&.Mui-focused": {
                  color: theme.palette.primary.main,
                },
              },
              "& .MuiInputBase-input": {
                color: theme.palette.text.disabled,
                cursor: "not-allowed",
                fontStyle: "italic",
                fontSize: "14px",
                fontWeight: "500",
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
          />
        );
      }}
      name={name}
    />
  );
};
