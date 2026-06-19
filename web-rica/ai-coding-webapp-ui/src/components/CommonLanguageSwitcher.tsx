/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import MenuItem from "@mui/material/MenuItem";
import type { SelectChangeEvent } from "@mui/material/Select";
import Select from "@mui/material/Select";
import Tooltip from "@mui/material/Tooltip";
import { LanguageSwitcherProps } from "../typings/component.props.ts";
import { useTranslation } from "react-i18next";
import { updateUserSettings } from "../pages/requests/user-request.ts";
import { Language } from "../typings/common.props.ts";

export const CommonLanguageSwitcher = ({
  selectionItems,
}: LanguageSwitcherProps) => {
  const { i18n, t } = useTranslation();

  const handleChange = async (event: SelectChangeEvent) => {
    await updateUserSettings({
      preferred_language: event.target.value as Language,
    });
    i18n.changeLanguage(event.target.value);
  };
  return (
    <Tooltip title={t("switchLanguage")} placement="right-end" arrow>
      <Select
        size="small"
        value={i18n.language}
        sx={{
          height: 32,
          border: 0,
          ".MuiOutlinedInput-notchedOutline": {
            border: "none", // Removes the border
          },
        }}
        onChange={handleChange}
        displayEmpty
        inputProps={{ "aria-label": "Without label" }}
      >
        {selectionItems &&
          selectionItems.map((selectionItem) => {
            return (
              <MenuItem
                sx={{ display: "flex", justifyContent: "center" }}
                value={selectionItem.value}
                key={`sel-${selectionItem.value}`}
              >
                {selectionItem.name}
              </MenuItem>
            );
          })}
      </Select>
    </Tooltip>
  );
};
