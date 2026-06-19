/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { ToolbarItems } from "../../../components/CommonToolbar.tsx";
import SearchIcon from "@mui/icons-material/Search";
import { InputAdornment } from "@mui/material";
import React, { useCallback, useMemo } from "react";
import { useDispatch } from "react-redux";
import { updateDrawerConfig } from "../../../store/reducers/drawer.slice.ts";
import { AddOutlined } from "@mui/icons-material";
import { CreateEditKnowledgeBaseDetail } from "../components/KnowledgeBaseEditFileDetail.tsx";
import { useTranslation } from "react-i18next";
import { FileManagement } from "../../../store/api-slices/fileApiSlice.ts";

export const useKnowledgeBaseToolBarItems = (
  searchValue: string,
  fileList: FileManagement[],
  handleOnChangeInput: (event: React.ChangeEvent<HTMLInputElement>) => void,
) => {
  const dispatch = useDispatch();
  const { t } = useTranslation();
  const handleOnclick = useCallback(
    (ev: React.MouseEvent) => {
      ev.preventDefault();
      dispatch(
        updateDrawerConfig({
          isOpen: true,
          children: <CreateEditKnowledgeBaseDetail fileList={fileList} />,
          title: t("title.createKnowledgeBase"),
          drawerWidth: "40%",
          maxDrawerWidth: "40%",
          anchor: "right",
          keepMounted: false,
          isOverflow: false,
        }),
      );
    },
    [dispatch, fileList, t],
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
          label: t("title.searchKnowledgeBase"),
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
          startIcon: <AddOutlined />,
        },
        label: t("title.createKnowledgeBase"),
      },
    ];
    return toolbarItems;
  }, [searchValue, handleOnclick, handleOnChangeInput]);
};
