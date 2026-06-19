/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */
import AccessTimeOutlinedIcon from "@mui/icons-material/AccessTimeOutlined";
import ArticleOutlinedIcon from "@mui/icons-material/ArticleOutlined";

import DeleteIcon from "@mui/icons-material/Delete";
import EditIcon from "@mui/icons-material/Edit";
import ShareIcon from "@mui/icons-material/Share";
import { GridColDef, GridRowClassNameParams } from "@mui/x-data-grid";
import React, { useMemo, useRef } from "react";
import { useTranslation } from "react-i18next";
import { useDispatch } from "react-redux";
import {
  KnowledgeBaseAll,
  useDeleteKnowledgeBaseWithIdMutation,
} from "../../../store/api-slices/knowledgeBaseAPISlice.ts";
import { updateDialogConfig } from "../../../store/reducers/dialogComponent.slice.ts";
import { updateDrawerConfig } from "../../../store/reducers/drawer.slice.ts";
import {
  COMMON_DISPLAY_FORMAT_WITH_HOUR_24,
  normalizeDateString,
} from "../../../utils/dayjs.ts";
import { CreateEditKnowledgeBaseDetail } from "../components/KnowledgeBaseEditFileDetail.tsx";
import { Box, Typography, Tooltip } from "@mui/material";

import { useCustomPagination } from "../../../hooks/useCustomPagination.tsx";
import { FileManagement } from "../../../store/api-slices/fileApiSlice.ts";
import { File } from "../../../typings/component.props.ts";
import { FilesCell } from "../components/FilesCell.tsx";

export const useKnowledgeBaseDataTable = (
  fileList: FileManagement[],
  onShare?: (knowledgeBaseId: string) => void,
) => {
  const ref = useRef<HTMLTableElement | null>(null);

  const { t } = useTranslation();
  const { page, query, entries } = useCustomPagination();
  const [deleteKBWithId] = useDeleteKnowledgeBaseWithIdMutation();
  const dispatch = useDispatch();
  const dataSource: GridColDef<KnowledgeBaseAll>[] = useMemo<
    GridColDef<KnowledgeBaseAll>[]
  >(() => {
    return ref?.current?.offsetWidth
      ? [
          {
            field: "name",
            headerName: t("table.header.kb_name"),
            width: (ref?.current?.offsetWidth as number) * 0.2 || 150,
            renderCell: (params) => (
              <Box
                sx={{
                  display: "flex",
                  alignItems: "center",
                  height: "100%",
                  gap: 1,
                }}
              >
                <Box
                  sx={{
                    display: "flex",
                    alignItems: "center",
                    justifyContent: "center",
                    width: 32,
                    height: 32,
                    borderRadius: "8px",
                    backgroundColor: "rgba(91,89,211,0.15)",
                    color: "#7875E8",
                    flexShrink: 0,
                  }}
                >
                  <ArticleOutlinedIcon sx={{ fontSize: 18 }} />
                </Box>
                <Typography
                  sx={{
                    fontSize: "13px",
                    fontWeight: 600,
                    color: "text.primary",
                    overflow: "hidden",
                    textOverflow: "ellipsis",
                    whiteSpace: "nowrap",
                  }}
                >
                  {params.value}
                </Typography>
              </Box>
            ),
          },
          {
            field: "created_at",
            headerName: t("table.header.created_at"),
            width: (ref?.current?.offsetWidth as number) * 0.2 || 150,
            renderCell: (params: any) => {
              return (
                <Box
                  sx={{
                    display: "flex",
                    alignItems: "center",
                    height: "100%",
                  }}
                >
                  <Box sx={{ display: "flex", alignItems: "center", gap: 0.5 }}>
                    <AccessTimeOutlinedIcon
                      sx={{
                        fontSize: 14,
                        color: "#06b6d4",
                      }}
                    />
                    <Typography
                      sx={{
                        fontSize: "13px",
                        fontWeight: 600,
                        color: "text.primary",
                      }}
                    >
                      {
                        normalizeDateString(
                          params.value,
                          true,
                          "en",
                          COMMON_DISPLAY_FORMAT_WITH_HOUR_24,
                        )?.split(" ")[0]
                      }
                    </Typography>
                    <Typography
                      sx={{
                        fontSize: "13px",
                        color: "text.secondary",
                      }}
                    >
                      {normalizeDateString(
                        params.value,
                        true,
                        "en",
                        COMMON_DISPLAY_FORMAT_WITH_HOUR_24,
                      )
                        ?.split(" ")
                        .slice(1)
                        .join(" ")}
                    </Typography>
                  </Box>
                </Box>
              );
            },
          },
          {
            field: "files",
            headerName: t("table.header.documents"),
            width: (ref?.current?.offsetWidth as number) * 0.2 || 150,
            renderCell: (params) => {
              return <FilesCell files={params.value as File[]} />;
            },
          },
          {
            field: "owner",
            headerName: t("table.header.owner"),
            width: (ref?.current?.offsetWidth as number) * 0.2 || 150,
            renderCell: (params) => (
              <Box
                sx={{
                  display: "flex",
                  alignItems: "center",
                  height: "100%",
                }}
              >
                <Typography
                  sx={{
                    fontSize: "13px",
                    fontWeight: 600,
                    color: "text.primary",
                    padding: "4px 12px",
                    borderRadius: "6px",
                    backgroundColor: "rgba(255,255,255,0.08)",
                    border: "1px solid",
                    borderColor: "divider",
                  }}
                >
                  {params.value}
                </Typography>
              </Box>
            ),
          },
          {
            field: "action",
            headerName: t("table.header.actions"),
            width: (ref?.current?.offsetWidth as number) * 0.1,
            renderCell: (params) => {
              const handleDelete = async () => {
                dispatch(
                  updateDialogConfig({
                    isOpen: true,
                    title: t("title.KB.deleteKB"),
                    fullWidth: false,
                    hasConfirmButton: true,
                    hasCancelButton: true,
                    hasAction: true,
                    children: (
                      <span>{t("text.knowledgeBase.confirmDeleteKB")}</span>
                    ),
                    onSubmit: async () => {
                      await deleteKBWithId(params.row.id);
                    },
                  }),
                );
              };
              const handleOnclick = (ev: React.MouseEvent) => {
                ev.preventDefault();
                dispatch(
                  updateDrawerConfig({
                    isOpen: true,
                    children: (
                      <CreateEditKnowledgeBaseDetail
                        fileList={fileList}
                        knowledgeBaseId={params.row.id}
                        currentUserRole={params.row.current_user_role}
                      />
                    ),
                    title: t("title.KB.editKnowledgeBase"),
                    drawerWidth: "40%",
                    maxDrawerWidth: "40%",
                    anchor: "right",
                    keepMounted: false,
                    isOverflow: false,
                    onClose: () => {
                      dispatch(
                        updateDrawerConfig({
                          isOpen: false,
                          children: null,
                        }),
                      );
                    },
                  }),
                );
              };

              return (
                <Box
                  sx={{
                    display: "flex",
                    gap: 1,
                    alignItems: "center",
                    height: "100%",
                  }}
                >
                  <Tooltip
                    title={
                      params.row.can_edit
                        ? t("title.KB.editKnowledgeBase")
                        : t(
                            "You can not {{action}} this resource due to permission.",
                            { action: t("edit") },
                          )
                    }
                    arrow
                  >
                    <Box
                      onClick={(e) => {
                        if (params.row.can_edit) {
                          handleOnclick(e);
                        }
                      }}
                      sx={{
                        cursor: params.row.can_edit ? "pointer" : "not-allowed",
                        opacity: params.row.can_edit ? "1" : "0.5",
                        display: "inline-flex",
                        alignItems: "center",
                        justifyContent: "center",
                        width: 32,
                        height: 32,
                        borderRadius: "8px",
                        backgroundColor: "rgba(91,89,211,0.15)",
                        border: "1px solid rgba(91,89,211,0.4)",
                        transition: "all 0.2s ease",
                        color: "#7875E8",
                        "&:hover": {
                          backgroundColor: "rgba(91,89,211,0.25)",
                          transform: "scale(1.1)",
                        },
                        "&:active": {
                          transform: "scale(0.95)",
                        },
                      }}
                    >
                      <EditIcon sx={{ fontSize: 16 }} />
                    </Box>
                  </Tooltip>

                  <Tooltip
                    title={
                      params.row.can_share
                        ? t("Share Knowledge Base")
                        : t(
                            "You can not {{action}} this resource due to permission.",
                            { action: t("share") },
                          )
                    }
                    arrow
                  >
                    <Box
                      onClick={() => {
                        if (params.row.can_share) {
                          onShare?.(params.row.id);
                        }
                      }}
                      sx={{
                        cursor: params.row.can_share
                          ? "pointer"
                          : "not-allowed",
                        opacity: params.row.can_share ? "1" : "0.5",
                        display: "inline-flex",
                        alignItems: "center",
                        justifyContent: "center",
                        width: 32,
                        height: 32,
                        borderRadius: "8px",
                        backgroundColor: "rgba(16,185,129,0.15)",
                        border: "1px solid rgba(16,185,129,0.4)",
                        transition: "all 0.2s ease",
                        color: "#10B981",
                        "&:hover": {
                          backgroundColor: "rgba(16,185,129,0.25)",
                          transform: "scale(1.1)",
                        },
                        "&:active": {
                          transform: "scale(0.95)",
                        },
                      }}
                    >
                      <ShareIcon sx={{ fontSize: 16 }} />
                    </Box>
                  </Tooltip>

                  <Tooltip
                    title={
                      params.row.can_delete
                        ? t("title.KB.deleteKB")
                        : t(
                            "You can not {{action}} this resource due to permission.",
                            { action: t("delete") },
                          )
                    }
                    arrow
                  >
                    <Box
                      onClick={() => {
                        if (params.row.can_delete) {
                          handleDelete();
                        }
                      }}
                      sx={{
                        cursor: params.row.can_delete
                          ? "pointer"
                          : "not-allowed",
                        opacity: params.row.can_delete ? "1" : "0.5",
                        display: "inline-flex",
                        alignItems: "center",
                        justifyContent: "center",
                        width: 32,
                        height: 32,
                        borderRadius: "8px",
                        backgroundColor: "rgba(248,113,113,0.15)",
                        border: "1px solid rgba(248,113,113,0.4)",
                        transition: "all 0.2s ease",
                        color: "#F87171",
                        "&:hover": {
                          backgroundColor: "rgba(248,113,113,0.25)",
                          transform: "scale(1.1) rotate(5deg)",
                          boxShadow: "0 4px 6px rgba(248, 113, 113, 0.2)",
                        },
                        "&:active": {
                          transform: "scale(0.95)",
                        },
                      }}
                    >
                      <DeleteIcon sx={{ fontSize: 16 }} />
                    </Box>
                  </Tooltip>
                </Box>
              );
            },
          },
        ]
      : [];
  }, [t, deleteKBWithId, dispatch, page, entries, query, fileList, onShare]);

  const getRowClassName = (params: GridRowClassNameParams) => {
    return params.row.is_default ? "default-kb-row" : "";
  };

  return { dataSource, ref, getRowClassName };
};
