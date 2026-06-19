/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */
import Checkbox from "@mui/material/Checkbox";
import Chip from "@mui/material/Chip";
import FormControl from "@mui/material/FormControl";
import FormHelperText from "@mui/material/FormHelperText";
import InputLabel from "@mui/material/InputLabel";
import ListItemText from "@mui/material/ListItemText";
import MenuItem from "@mui/material/MenuItem";
import Select from "@mui/material/Select";

import { Control, Controller } from "react-hook-form";
import { FormItem } from "../../typings/common.props.ts";
import { Fragment } from "react";
import Box from "@mui/material/Box/Box";
import { SelectProps } from "@mui/material/Select/Select";
import { Placement } from "../../constants/common.ts";
import Tooltip from "@mui/material/Tooltip";
import Typography from "@mui/material/Typography";

export interface SecondaryText {
  content: string;
  placement: Placement;
}

export interface SelectItem {
  name: string;
  value: string;
  disabled?: boolean;
  title?: string;
  secondaryText?: SecondaryText;
}

interface SelectionProps<T> {
  label: string;
  formItem: FormItem<T>;
  control: Control<any>;
  name: string;
  externalHandleChange?: (value: string) => void;
}

type CommonSelectionProps<T> = SelectionProps<T> & SelectProps;

export const CommonSelection = <T,>({
  label,
  formItem,
  control,
  multiple,
  name,
  externalHandleChange,
  ...props
}: CommonSelectionProps<T>) => {
  const labelId = `common-select-label-${formItem.name}`;
  const selectId = `common-select-${formItem.name}`;

  return (
    <Controller
      name={name}
      control={control}
      render={({ field, fieldState: { error } }) => {
        // Ensure value is always an array for multiple select
        const fieldValue = multiple
          ? Array.isArray(field.value)
            ? field.value
            : field.value
              ? [field.value]
              : []
          : field.value;

        const chipRenderer =
          multiple && Array.isArray(fieldValue)
            ? (formItem.values as SelectItem[]).reduce((acc, item) => {
                return fieldValue.includes(item.value)
                  ? [...acc, item.name]
                  : acc;
              }, [] as string[])
            : [];

        return (
          <FormControl
            fullWidth
            error={!!error}
            className="w-full"
            sx={(theme) => ({
              margin: 0,
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
            })}
          >
            {(formItem.properties as SelectProps).multiple ? (
              <Fragment>
                <InputLabel id={labelId}>{label}</InputLabel>
                <Select
                  multiple={multiple}
                  labelId={labelId}
                  id={selectId}
                  label={label}
                  name={field.name}
                  value={fieldValue}
                  onBlur={field.onBlur}
                  ref={field.ref}
                  renderValue={() => (
                    <Box sx={{ display: "flex", flexWrap: "wrap", gap: 0.5 }}>
                      {chipRenderer.map((chip) => (
                        <Chip
                          key={chip}
                          label={chip}
                          size="small"
                          sx={(theme) => ({
                            backgroundColor: theme.palette.primary.dark,
                            color: theme.palette.common.white,
                            "& .MuiChip-deleteIcon": {
                              color: theme.palette.primary.light,
                            },
                          })}
                        />
                      ))}
                    </Box>
                  )}
                  {...props}
                  onChange={(event) => {
                    console.log(
                      "🚀 ~ Select onChange ~ event.target.value:",
                      event.target.value,
                    );
                    field.onChange(event.target.value);
                    if (typeof externalHandleChange === "function") {
                      externalHandleChange(event.target.value as string);
                    }
                  }}
                  MenuProps={{
                    PaperProps: {
                      sx: {
                        borderRadius: "12px",
                        boxShadow: "0 10px 25px rgba(0, 0, 0, 0.4)",
                        maxHeight: "300px",
                        "& .MuiMenuItem-root": {
                          borderRadius: "8px",
                          margin: "2px 8px",
                          "&:hover": {
                            backgroundColor: "action.hover",
                          },
                          "&.Mui-selected": {
                            backgroundColor: "primary.dark",
                            "&:hover": {
                              backgroundColor: "primary.main",
                            },
                          },
                        },
                      },
                    },
                  }}
                >
                  {(formItem.values as SelectItem[]).map((item, index) => (
                    <Tooltip
                      key={`${item.value}-${index}`}
                      title={item.disabled ? item.title || "" : ""}
                      arrow
                      placement="right"
                    >
                      <span>
                        <MenuItem
                          value={item.value}
                          disabled={item.disabled}
                          sx={{
                            opacity: item.disabled ? 0.5 : 1,
                            cursor: item.disabled ? "not-allowed" : "pointer",
                          }}
                          onClick={() => {
                            if (item.disabled) return;

                            const currentValue = Array.isArray(fieldValue)
                              ? fieldValue
                              : [];
                            let newValue: string[];

                            if (currentValue.includes(item.value)) {
                              // Remove item from array
                              newValue = currentValue.filter(
                                (val) => val !== item.value,
                              );
                            } else {
                              // Add item to array
                              newValue = [...currentValue, item.value];
                            }

                            field.onChange(newValue);
                            if (typeof externalHandleChange === "function") {
                              externalHandleChange(newValue.join(","));
                            }
                          }}
                        >
                          <Checkbox
                            checked={
                              Array.isArray(fieldValue) &&
                              fieldValue.includes(item.value)
                            }
                            disabled={item.disabled}
                            sx={(theme) => ({
                              color: theme.palette.text.secondary,
                              "&.Mui-checked": {
                                color: theme.palette.primary.main,
                              },
                            })}
                          />
                          <Box
                            sx={{
                              display: "flex",
                              flexDirection: "column",
                              flex: 1,
                            }}
                          >
                            <ListItemText primary={item.name} />
                            {item.secondaryText && (
                              <Typography
                                variant="caption"
                                sx={(theme) => ({
                                  color: item.disabled
                                    ? theme.palette.text.disabled
                                    : theme.palette.text.secondary,
                                  fontSize: "0.75rem",
                                })}
                              >
                                {item.secondaryText.content}
                              </Typography>
                            )}
                          </Box>
                        </MenuItem>
                      </span>
                    </Tooltip>
                  ))}
                </Select>
              </Fragment>
            ) : (
              <Fragment>
                <InputLabel id={labelId}>{label}</InputLabel>
                <Select
                  labelId={labelId}
                  id={selectId}
                  label={label}
                  {...field}
                  {...props}
                  onChange={(ev) => {
                    if ((formItem?.properties as SelectProps)?.onChange) {
                      (formItem?.properties as SelectProps)?.onChange?.(
                        ev,
                        null,
                      );
                    }
                    field.onChange(ev);
                  }}
                  MenuProps={{
                    PaperProps: {
                      sx: {
                        borderRadius: "12px",
                        boxShadow: "0 10px 25px rgba(0, 0, 0, 0.4)",
                        maxHeight: "300px",
                        "& .MuiMenuItem-root": {
                          borderRadius: "8px",
                          margin: "2px 8px",
                          "&:hover": {
                            backgroundColor: "action.hover",
                          },
                          "&.Mui-selected": {
                            backgroundColor: "primary.dark",
                            "&:hover": {
                              backgroundColor: "primary.main",
                            },
                          },
                        },
                      },
                    },
                  }}
                >
                  {(formItem.values as SelectItem[]).map((item, index) => (
                    <MenuItem
                      value={item.value}
                      key={`${item.value}-${index}`}
                      onClick={() => {
                        field.onChange(item.value);
                        if (typeof externalHandleChange === "function") {
                          externalHandleChange(item.value);
                        }
                      }}
                    >
                      {item.name}
                    </MenuItem>
                  ))}
                </Select>
              </Fragment>
            )}
            {error && (
              <FormHelperText
                sx={(theme) => ({
                  marginLeft: 1,
                  fontSize: "0.875rem",
                  color: theme.palette.error.main,
                  fontWeight: 500,
                })}
              >
                {error.message}
              </FormHelperText>
            )}
          </FormControl>
        );
      }}
    />
  );
};
