/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { useMemo } from "react";

import InputAdornment from "@mui/material/InputAdornment";
import BadgeOutlinedIcon from "@mui/icons-material/BadgeOutlined";
import { FormItem } from "../../../../typings/common.props.ts";
import { useTranslation } from "react-i18next";
import {
  useGetModelDatabricksQuery,
  useGetModelRolesListQuery,
} from "../../../../store/api-slice/modelAPISlice.ts";

export const useModelDatum = () => {
  const { t } = useTranslation();
  const { data: modelDatabricksList } = useGetModelDatabricksQuery();
  const { data: modelRoles } = useGetModelRolesListQuery();
  return useMemo(() => {
    const formItem: FormItem<Partial<any>>[] = [
      {
        type: "input",
        label: t("text.model.name.label") || "Name",
        name: "name",
        values: "",
        properties: {
          placeholder: t("text.model.placeholder.name") || "Type name of Topic",
          type: "text",
          value: "",
          label: t("text.model.label.name") || "Name",
          sx: {
            minWidth: "60%",
          },

          slotProps: {
            input: {
              startAdornment: (
                <InputAdornment position="start">
                  <BadgeOutlinedIcon />
                </InputAdornment>
              ),
            },
          },
        },
      },

      {
        type: "single-autocomplete",
        label: t("text.model.label.description") || "Description",
        name: "model_path",
        values: modelDatabricksList || [],
        autoCompleteOptions: {
          getOptionLabel: (option) =>
            typeof option === "string" ? option : (option as any).name || "",
          isOptionEqualToValue: (option, val) => {
            if (!option || !val) return false;
            return (option as any).value === (val as any).value;
          },
          getOptionKey: (option) => (option as any)?.value || "",
        },
        properties: {
          label: t("text.model.label.model_path"),
          autoComplete: "on",
        },
      },
      {
        type: "autocomplete",
        label: t("text.model.label.roles") || "Description",
        name: "roles",
        values: modelRoles,

        autoCompleteOptions: {
          multiple: true,
          getOptionLabel: (option) => option.name || "",
          getOptionKey: (option) => option.value || "",
          isOptionEqualToValue: (option, value) => option.value === value.value,
        },
        properties: {
          label: t("text.model.label.roles"),
          autoComplete: "on",
        },
      },
    ];
    const tokenConfigItems: FormItem<Partial<any>>[] = [
      {
        type: "input",
        label: t("text.model.label.max_tokens"),
        name: "max_tokens",
        values: "",
        properties: {
          placeholder: t("text.model.placeholder.max_tokens"),
          type: "number",
          value: "",
          label: t("text.model.label.max_tokens"),
          sx: {
            minWidth: "60%",
          },

          slotProps: {
            input: {
              startAdornment: (
                <InputAdornment position="start">
                  <BadgeOutlinedIcon />
                </InputAdornment>
              ),
            },
          },
        },
      },
      {
        type: "input",
        label: t("text.model.label.temperature"),
        name: "temperature",
        values: "",
        properties: {
          placeholder: t("text.model.placeholder.temperature"),
          type: "number",
          value: "",
          label: t("text.model.label.temperature"),
          inputProps: {
            min: 0,
            max: 2,
            step: 0.1,
          },
          sx: {
            minWidth: "60%",
          },

          slotProps: {
            input: {
              startAdornment: (
                <InputAdornment position="start">
                  <BadgeOutlinedIcon />
                </InputAdornment>
              ),
            },
          },
        },
      },
      {
        type: "input",
        label: t("text.model.label.context_length"),
        name: "context_length",
        values: "",
        properties: {
          placeholder: t("text.model.placeholder.context_length"),
          type: "number",
          value: "",
          label: t("text.model.label.context_length"),
          inputProps: {
            step: 1,
            min: 1,
          },
          sx: {
            minWidth: "60%",
          },

          slotProps: {
            input: {
              startAdornment: (
                <InputAdornment position="start">
                  <BadgeOutlinedIcon />
                </InputAdornment>
              ),
            },
          },
        },
      },
      {
        type: "input",
        label: t("text.model.label.threshold"),
        name: "threshold",
        values: "",
        properties: {
          placeholder: t("text.model.placeholder.threshold"),
          type: "number",
          value: "",
          label: t("text.model.label.threshold"),
          inputProps: {
            step: 1,
            min: 1,
          },
          sx: {
            minWidth: "60%",
          },

          slotProps: {
            input: {
              startAdornment: (
                <InputAdornment position="start">
                  <BadgeOutlinedIcon />
                </InputAdornment>
              ),
            },
          },
        },
      },
      {
        type: "input",
        label: t("text.model.label.consumption_range_hours"),
        name: "consumption_range_hours",
        values: "",
        properties: {
          placeholder: t("text.model.placeholder.consumption_range_hours"),
          type: "number",
          value: "",
          label: t("text.model.label.consumption_range_hours"),
          inputProps: {
            step: 1,
            min: 1,
          },
          sx: {
            minWidth: "60%",
          },

          slotProps: {
            input: {
              startAdornment: (
                <InputAdornment position="start">
                  <BadgeOutlinedIcon />
                </InputAdornment>
              ),
            },
          },
        },
      },
    ];
    return { formItem, tokenConfigItems };
  }, [t, modelDatabricksList, modelRoles]);
};
