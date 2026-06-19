/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { useMemo, useState } from "react";
import { Box, Chip } from "@mui/material";
import { FormItem } from "../../../typings/common.props.ts";
import { useTranslation } from "react-i18next";
import {
  FileManagement,
  FileStatus,
} from "../../../store/api-slices/fileApiSlice.ts";
import { useGetFilesQuery } from "../../../store/api-slices/fileApiSlice.ts";
import { ENTRIES_LARGE_RECORDS } from "../../../constants/common.ts";
import useDebouncedCallback from "../../../hooks/useDebounceCallback.tsx";
import { useGetKnowledgeBaseDetailQuery } from "../../../store/api-slices/knowledgeBaseAPISlice.ts";
interface KnowledgeBaseDatum {
  currentUserRole?: string;
  isEdit?: boolean;
  knowledgeBaseId?: string;
}

export const useKnowledgeBaseDatum = ({
  currentUserRole,
  isEdit,
  knowledgeBaseId,
}: KnowledgeBaseDatum) => {
  const { t } = useTranslation();
  const [searchKeyword, setSearchKeyword] = useState("");
  const { data: knowledgeDetail } = useGetKnowledgeBaseDetailQuery(
    knowledgeBaseId!,
    {
      skip: !knowledgeBaseId,
    },
  );
  const {
    data: fileList,
    isSuccess,
    isFetching,
  } = useGetFilesQuery({
    query: searchKeyword,
    page: 0,
    entries: ENTRIES_LARGE_RECORDS,
  });

  const onChangeAutoCompleteHandle = useDebouncedCallback((value: string) => {
    setSearchKeyword(value);
  }, 500);

  return useMemo(() => {
    // const realFileList =
    const formItem: FormItem<Partial<any>>[] = [
      {
        type: "input",
        label: t("text.knowledgeBase.label.name"),
        name: "name",
        values: "",
        properties: {
          disabled: isEdit && currentUserRole !== "owner",
          placeholder: t("text.knowledgeBase.placeholder.name"),
          type: "text",
          value: "",
          label: t("text.knowledgeBase.label.name"),
          sx: {
            minWidth: "60%",
          },
        },
      },
      {
        type: "input",
        label: t("text.knowledgeBase.label.description"),
        name: "description",
        values: "",
        properties: {
          disabled: isEdit && currentUserRole !== "owner",
          placeholder: t("text.knowledgeBase.placeholder.description"),
          type: "text",
          value: "",
          label: t("text.knowledgeBase.label.description"),
          multiline: true,
          minRows: 3,
          sx: {
            minWidth: "60%",
          },
        },
      },
      {
        type: "autocomplete",
        label: "Files",
        name: "files",
        values: fileList?.data || [],
        autoCompleteOptions: {
          multiple: true,
          getOptionLabel: (option) => {
            if (typeof option === "string") {
              if (isSuccess && fileList?.data) {
                const found = fileList.data.find(
                  (file: any) => file.id === option,
                );
                return found?.file_name || option;
              }
              return option;
            }
            return (option as any).file_name || "No data";
          },
          getOptionKey: (option) =>
            typeof option === "string" ? option : (option as any).id || "",
          isOptionEqualToValue: (option, value) => {
            const optionId =
              typeof option === "string" ? option : (option as any).id;
            const valueId =
              typeof value === "string" ? value : (value as any).id;
            return optionId === valueId;
          },
          renderOption: (props, option: Partial<FileManagement>) => {
            const statusFile = option.status;
            const isDisabled =
              knowledgeDetail?.files.some((f) => f.file_id === option.id) ||
              (option as any).status !== FileStatus.completed;
            return (
              <Box
                component="li"
                {...props}
                style={{
                  fontSize: 12,
                  opacity: isDisabled ? 0.5 : 1,
                  pointerEvents: isDisabled ? "none" : "auto",
                  display: "flex",
                  alignItems: "center",
                  gap: 8,
                }}
              >
                {(option as any).file_name}
                {isDisabled && (
                  <Chip
                    label={
                      statusFile === FileStatus.failed
                        ? t(FileStatus.failed)
                        : t("You added this file before")
                    }
                    size="small"
                    color={
                      (option as any).status === FileStatus.failed
                        ? "error"
                        : "info"
                    }
                    sx={{ height: 20, fontSize: 10 }}
                  />
                )}
              </Box>
            );
          },
          getOptionDisabled: (option) =>
            (option as any).status !== FileStatus.completed,
        },
        properties: {
          onChange: (ev: React.ChangeEvent<HTMLInputElement>) =>
            onChangeAutoCompleteHandle(ev.target.value),
          value: searchKeyword,
          label: "Files",
          autoComplete: "off",
          loading: isFetching,
        },
      },
    ];
    return formItem;
  }, [
    fileList,
    isEdit,
    t,
    knowledgeDetail,
    searchKeyword,
    currentUserRole,
    isFetching,
  ]);
};
