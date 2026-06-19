/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */
import { useEffect } from "react";
import {
  FormControl,
  MenuItem,
  Select,
  SelectChangeEvent,
} from "@mui/material";
import { SelectionItem } from "../../typings/component.props.ts";
import { FileUploadActionValue } from "../../pages/files-management/constants/file.const.ts";
import { useCallback, useState } from "react";

export interface CommonSelectWithoutFormControlProps {
  checkId: string;
  actions: SelectionItem[];
  disabled?: boolean;
  defaultValue?: string;
  handleUpdate?: (id: string, value: string) => void;
}

export const CommonSelectWithoutFormControl = ({
  checkId,
  actions,
  disabled = false,
  defaultValue,
  handleUpdate,
}: CommonSelectWithoutFormControlProps) => {
  const [action, setAction] = useState<string>(
    defaultValue || FileUploadActionValue.REPLACE,
  );

  useEffect(() => {
    if (defaultValue && defaultValue !== action) {
      setAction(defaultValue);
    }
  }, [defaultValue, action]);

  const handleChange = useCallback(
    (ev: SelectChangeEvent<string>) => {
      if (disabled) return;

      setAction(ev.target.value);
      if (handleUpdate) {
        handleUpdate(checkId, ev.target.value);
      }
    },
    [checkId, handleUpdate, disabled],
  );

  return (
    <FormControl size="small" sx={{ mx: 1, minWidth: 100 }}>
      <Select
        variant="outlined"
        value={action}
        onChange={handleChange}
        disabled={disabled}
        displayEmpty
        sx={{
          "& .MuiSelect-select": {
            color: disabled ? "text.disabled" : "primary.main",
            fontWeight: 500,
            py: 0.5,
          },
          "&.Mui-disabled": {
            backgroundColor: "action.disabledBackground",
            "& .MuiSelect-select": {
              color: "text.disabled",
            },
          },
        }}
      >
        {actions.map((option) => (
          <MenuItem
            key={option.value}
            value={option.value}
            disabled={
              disabled && option.value === FileUploadActionValue.REPLACE
            }
          >
            {option.name}
          </MenuItem>
        ))}
      </Select>
    </FormControl>
  );
};
