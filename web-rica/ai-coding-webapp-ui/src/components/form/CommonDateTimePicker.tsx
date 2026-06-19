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
import { LocalizationProvider } from "@mui/x-date-pickers/LocalizationProvider";
import {
  DateTimePicker,
  DateTimePickerProps,
} from "@mui/x-date-pickers/DateTimePicker";
import { AdapterDayjs } from "@mui/x-date-pickers/AdapterDayjs";
import { dateString2Dayjs } from "../../utils/dayjs.ts";
import { useMediaQuery } from "@mui/material";

interface FormItem {
  name: string;
  label?: string;
  properties?: any;
}

interface DateTimePickerFieldProps<TType extends FieldValues> {
  item: FormItem;
  control: Control<TType>;
  errors: FieldErrors<TType>;
  index: number;
}

const CommonDateTimePicker = <TType extends Record<string, any>>({
  item,
  control,
  errors,
  index,
}: DateTimePickerFieldProps<TType>) => {
  const isLaptopScreen = useMediaQuery("(max-width: 1300px)");
  return (
    <Controller
      key={index}
      name={item.name as Path<TType>}
      control={control}
      render={({ field }) => {
        // Convert field value to dayjs object or null
        const dateValue = field?.value
          ? dateString2Dayjs(field.value as string)
          : null;

        return (
          <LocalizationProvider dateAdapter={AdapterDayjs}>
            <DateTimePicker
              label={(item?.label || "Date Time Picker") as string}
              value={dateValue}
              onChange={(newValue) => {
                if (!newValue) {
                  field.onChange(null);
                  return;
                }
                field.onChange(newValue.format("YYYY-MM-DDTHH:mm:ss"));
              }}
              {...(item.properties as DateTimePickerProps)}
              format="DD/MM/YYYY HH:mm"
              ampm={false}
              sx={(theme) => ({
                "& .MuiPickersInputBase-root": {
                  borderRadius: "12px",
                  backgroundColor: `${theme.palette.background.paper} !important`,
                  transition: "all 0.2s ease-in-out",
                  "&:hover": {
                    backgroundColor: `${theme.palette.action.hover} !important`,
                    transform: "translateY(-1px)",
                    boxShadow: "0 4px 12px rgba(0, 0, 0, 0.3)",
                  },
                  "&.Mui-focused": {
                    backgroundColor: `${theme.palette.background.paper} !important`,
                  },
                },

                "& .MuiPickersInputBase-input": {
                  fontStyle: "italic !important",
                },

                "& .MuiPickersOutlinedInput-notchedOutline": {
                  borderColor: theme.palette.divider,
                },

                "& .MuiPickersOutlinedInput-root": {
                  borderRadius: "12px",
                  backgroundColor: `${theme.palette.background.paper} !important`,
                  "&:hover .MuiPickersOutlinedInput-notchedOutline": {
                    borderColor: theme.palette.text.secondary,
                  },
                  "&.Mui-focused .MuiPickersOutlinedInput-notchedOutline": {
                    borderColor: theme.palette.primary.main,
                    borderWidth: "2px",
                  },
                },

                "& .MuiInputLabel-root": {
                  fontWeight: 500,
                  "&.Mui-focused": {
                    color: theme.palette.primary.main,
                  },
                },
              })}
              slotProps={{
                textField: {
                  fullWidth: true,
                  error: !!errors[item.name as keyof TType],
                  helperText: errors[item.name as keyof TType]
                    ?.message as string,
                  margin: "normal",
                },
                popper: {
                  placement: isLaptopScreen ? "left-start" : "bottom-start",
                  modifiers: [
                    {
                      name: "preventOverflow",
                      enabled: true,
                      options: {
                        boundary: "viewport",
                        padding: 8,
                      },
                    },
                    {
                      name: "flip",
                      enabled: true,
                      options: {
                        fallbackPlacements: isLaptopScreen
                          ? ["left-end", "right-start", "bottom-start"]
                          : ["bottom-end", "top-start", "left-start"],
                      },
                    },
                  ],
                  sx: {
                    "& .MuiList-root": {
                      scrollbarWidth: "thin",
                      scrollbarColor: "#475569 transparent",
                      "&::-webkit-scrollbar": {
                        width: "6px",
                      },
                      "&::-webkit-scrollbar-track": {
                        background: "transparent",
                      },
                      "&::-webkit-scrollbar-thumb": {
                        backgroundColor: "#475569",
                        borderRadius: "3px",
                      },
                    },
                  },
                },
              }}
            />
          </LocalizationProvider>
        );
      }}
    />
  );
};

export default CommonDateTimePicker;
