/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import SearchIcon from "@mui/icons-material/Search";
import { InputAdornment } from "@mui/material";
import React, { useCallback, useMemo } from "react";
import { useDispatch } from "react-redux";
import { updateDrawerConfig } from "../../../store/reducers/drawer.slice.ts";
import FileUploadIcon from "@mui/icons-material/FileUpload";
import { ToolbarItems } from "../../../components/CommonToolbar.tsx";
import { CreateEditFileDetail } from "../components/CreateEditFileDetail.tsx";
import { useTranslation } from "react-i18next";

export const useFileToolbarItem = (
  searchValue: string,
  handleOnChangeInput: (event: React.ChangeEvent<HTMLInputElement>) => void,
  isUploadEnabled: boolean,
) => {
  const dispatch = useDispatch();
  const { t } = useTranslation();

  const handleOnclick = useCallback(
    (ev: React.MouseEvent) => {
      ev.preventDefault();
      if (!isUploadEnabled) return;
      dispatch(
        updateDrawerConfig({
          isOpen: true,
          children: <CreateEditFileDetail />,
          title: t("title.uploadFile"),
          drawerWidth: "40%",
          maxDrawerWidth: "40%",
          anchor: "right",
          keepMounted: false,
          isOverflow: false,
        }),
      );
    },
    [dispatch, isUploadEnabled, t],
  );
  return useMemo(() => {
    const toolbarItems: ToolbarItems<unknown>[] = [
      {
        name: "searchInput",
        type: "input",
        properties: {
          placeholder: `${t("searchText")}...`,
          type: "text",
          value: searchValue,
          onChange: handleOnChangeInput,
          label: t("title.searchFileName"),
          slotProps: {
            input: {
              startAdornment: (
                <InputAdornment position="start">
                  <SearchIcon />
                </InputAdornment>
              ),
            },
          },
          sx: {
            minWidth: "60%",
          },
        },
        label: "Search",
      },
      {
        name: "submitButton",
        type: "button",
        properties: {
          onClick: (ev: React.MouseEvent) => handleOnclick(ev),
          variant: "contained",
          size: "small",
          disabled: !isUploadEnabled,
          sx: {
            "&.Mui-disabled": {
              backgroundColor: "#bdbdbd",
              color: "#757575",
            },
          },
          startIcon: <FileUploadIcon />,
        },
        label: t("title.uploadFile"),
      },
    ];
    return toolbarItems;
  }, [t, searchValue, handleOnChangeInput, isUploadEnabled, handleOnclick]);
};
