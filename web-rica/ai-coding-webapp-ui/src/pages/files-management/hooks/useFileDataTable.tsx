/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { GridColDef, GridRenderCellParams } from "@mui/x-data-grid";
import { useMemo, useRef, useState } from "react";
import { useTranslation } from "react-i18next";
import {
  COMMON_DISPLAY_FORMAT_WITH_HOUR_24,
  normalizeDateString,
} from "../../../utils/dayjs.ts";
import { useDispatch } from "react-redux";
import { updateDialogConfig } from "../../../store/reducers/dialogComponent.slice.ts";
import { filesize } from "filesize";
import AccessTimeOutlinedIcon from "@mui/icons-material/AccessTimeOutlined";
import { fileStatus } from "../../../constants/common.ts";
import { useCustomPagination } from "../../../hooks/useCustomPagination.tsx";
import {
  FileStatus,
  useDeleteFileWithIDMutation,
} from "../../../store/api-slices/fileApiSlice.ts";
import DeleteIcon from "@mui/icons-material/Delete";
import VisibilityOutlinedIcon from "@mui/icons-material/VisibilityOutlined";
import Box from "@mui/material/Box/Box";
import Tooltip from "@mui/material/Tooltip";
import Typography from "@mui/material/Typography";
import { useIconGetter } from "../../../hooks/useIconGetter.tsx";

export interface PreviewFile {
  id: string;
  file_name: string;
}

export const useFileDataTable = () => {
  const ref = useRef<HTMLTableElement | null>(null);
  const { page, query, entries } = useCustomPagination();
  const [previewFile, setPreviewFile] = useState<PreviewFile | null>(null);
  const { t } = useTranslation();
  const { getIconFileType } = useIconGetter({
    fontSize: "small",
  });
  const [deleteFileWithId] = useDeleteFileWithIDMutation();
  const dispatch = useDispatch();
  const dataSource: GridColDef[] = useMemo<GridColDef[]>(() => {
    return [
      {
        field: "file_name",
        headerName: t("table.header.file_name"),
        width: (ref?.current?.clientWidth as number) * 0.2 || 200,
        renderCell: (params) => {
          const fileName = params.value ?? "";
          return (
            <Tooltip title={fileName} arrow placement="top">
              <Box
                display="flex"
                alignItems="center"
                height="100%"
                gap={1}
                sx={{
                  cursor: "pointer",
                  "&:hover": {
                    "& .file-name-text": {
                      color: "#7875E8",
                    },
                  },
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
                    backgroundColor: "rgba(59,159,255,0.15)",
                    color: "#6BBFFF",
                    flexShrink: 0,
                  }}
                >
                  {getIconFileType(fileName, false)}
                </Box>
                <Typography
                  noWrap
                  className="file-name-text"
                  sx={{
                    fontWeight: 600,
                    fontSize: "13px",
                    color: "text.primary",
                    overflow: "hidden",
                    textOverflow: "ellipsis",
                    transition: "color 0.2s",
                  }}
                >
                  {fileName}
                </Typography>
              </Box>
            </Tooltip>
          );
        },
      },
      {
        field: "owner",
        headerName: t("table.header.owner"),
        width: (ref?.current?.clientWidth as number) * 0.1 || 150,
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
        field: "uploaded_at",
        headerName: t("table.header.uploaded_at"),
        width: (ref?.current?.clientWidth as number) * 0.15 || 150,
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
      // {
      //   field: "modified",
      //   headerName: t("table.header.ingested_time"),
      //   width: (ref?.current?.clientWidth as number) * 0.15 || 150,
      //   renderCell: (params: any) => {
      //     return (
      //       <Box
      //         sx={{
      //           display: "flex",
      //           alignItems: "center",
      //           height: "100%",
      //         }}
      //       >
      //         <Box sx={{ display: "flex", alignItems: "center", gap: 0.5 }}>
      //           <AccessTimeOutlinedIcon
      //             sx={{
      //               fontSize: 14,
      //               color: "#10b981",
      //             }}
      //           />
      //           <Typography
      //             sx={{
      //               fontSize: "13px",
      //               fontWeight: 600,
      //               color: "#0f172a",
      //             }}
      //           >
      //             {
      //               normalizeDateString(
      //                 params.value,
      //                 true,
      //                 "en",
      //                 COMMON_DISPLAY_FORMAT_WITH_HOUR_24,
      //               )?.split(" ")[0]
      //             }
      //           </Typography>
      //           <Typography
      //             sx={{
      //               fontSize: "13px",
      //               color: "#64748b",
      //             }}
      //           >
      //             {normalizeDateString(
      //               params.value,
      //               true,
      //               "en",
      //               COMMON_DISPLAY_FORMAT_WITH_HOUR_24,
      //             )
      //               ?.split(" ")
      //               .slice(1)
      //               .join(" ")}
      //           </Typography>
      //         </Box>
      //       </Box>
      //     );
      //   },
      // },
      {
        field: "size",
        headerName: t("table.header.size"),
        width: (ref?.current?.clientWidth as number) * 0.1 || 180,
        renderCell: (params: any) => {
          const size = filesize(params.value as number);
          return (
            <Box
              sx={{
                display: "inline-flex",
                alignItems: "center",
                padding: "4px 12px",
                borderRadius: "6px",
                backgroundColor: "rgba(255,255,255,0.08)",
                border: "1px solid",
                borderColor: "divider",
              }}
            >
              <Typography
                sx={{
                  fontSize: "12px",
                  color: "text.secondary",
                  fontWeight: 600,
                  fontFamily: "monospace",
                  letterSpacing: "0.3px",
                }}
              >
                {size}
              </Typography>
            </Box>
          );
        },
      },
      {
        field: "total_pages",
        headerName: t("table.header.total_pages"),
        flex: 0.8,
        minWidth: 120,
        align: "center",
        headerAlign: "center",
        renderCell: (params: GridRenderCellParams) => (
          <Box
            sx={{
              display: "flex",
              alignItems: "center",
              justifyContent: "center",
              height: "100%",
            }}
          >
            <Box
              sx={{
                display: "inline-flex",
                alignItems: "center",
                justifyContent: "center",
                padding: "6px 12px",
                borderRadius: "8px",
                backgroundColor: "rgba(59,159,255,0.15)",
                border: "1px solid rgba(59,159,255,0.4)",
                minWidth: "40px",
              }}
            >
              <Typography
                sx={{
                  fontSize: "13px",
                  fontWeight: 600,
                  color: "#6BBFFF",
                  letterSpacing: "0.3px",
                }}
              >
                {params.value ?? "-"}
              </Typography>
            </Box>
          </Box>
        ),
      },
      {
        field: "status",
        headerName: t("table.header.status"),
        width: (ref?.current?.clientWidth as number) * 0.11 || 150,
        renderCell: (params: any) => {
          const chipRenderer = (status: FileStatus) => {
            const config = fileStatus[status] || fileStatus.default;

            return (
              <Box
                sx={{
                  display: "inline-flex",
                  alignItems: "center",
                  gap: 0.5,
                  padding: "4px 10px",
                  borderRadius: "16px",
                  backgroundColor: config.bgcolor,
                  border: `1px solid ${config.borderColor}`,
                  transition: "all 0.2s",
                  height: "24px",
                  width: "fit-content",
                }}
              >
                <span style={{ fontSize: "11px", lineHeight: 1 }}>
                  {config.icon}
                </span>
                <Typography
                  sx={{
                    fontSize: "11px",
                    fontWeight: 600,
                    color: config.color,
                    letterSpacing: "0.2px",
                    lineHeight: 1,
                  }}
                >
                  {config.label}
                </Typography>
              </Box>
            );
          };
          return chipRenderer(params.value as FileStatus);
        },
      },
      {
        field: "action",
        headerName: t("table.header.actions"),
        width: (ref?.current?.clientWidth as number) * 0.13 || 120,
        minWidth: 100,
        align: "center",
        renderCell: (params: any) => {
          const isCompleted = params.row.status === FileStatus.completed;

          const handleDelete = async () => {
            dispatch(
              updateDialogConfig({
                isOpen: true,
                title: t("title.deleteFile"),
                fullWidth: false,
                hasConfirmButton: true,
                hasCancelButton: true,
                hasAction: true,
                children: <span>{t("text.confirmDeleteFile")}</span>,
                onSubmit: async () => {
                  await deleteFileWithId({
                    id: params.row.id,
                    page,
                    entries,
                    query,
                  });
                },
              }),
            );
          };

          const handlePreview = () => {
            setPreviewFile({
              id: params.row.id,
              file_name: params.row.file_name,
            });
          };

          return (
            <Box
              sx={{
                display: "flex",
                alignItems: "center",
                justifyContent: "center",
                height: "100%",
                gap: 1,
              }}
            >
              {/* Preview button */}
              <Box
                onClick={isCompleted ? handlePreview : undefined}
                sx={{
                  display: "inline-flex",
                  alignItems: "center",
                  justifyContent: "center",
                  width: 32,
                  height: 32,
                  borderRadius: "8px",
                  backgroundColor: isCompleted
                    ? "rgba(59,159,255,0.15)"
                    : "rgba(255,255,255,0.05)",
                  border: `1px solid ${isCompleted ? "rgba(59,159,255,0.4)" : "rgba(255,255,255,0.12)"}`,
                  cursor: isCompleted ? "pointer" : "not-allowed",
                  opacity: isCompleted ? 1 : 0.4,
                  transition: "all 0.2s ease",
                  color: isCompleted ? "#6BBFFF" : "#94A3B8",
                  "&:hover": isCompleted
                    ? {
                        backgroundColor: "rgba(59,159,255,0.25)",
                        transform: "scale(1.1)",
                        boxShadow: "0 4px 6px rgba(59, 159, 255, 0.2)",
                      }
                    : {},
                  "&:active": isCompleted ? { transform: "scale(0.95)" } : {},
                }}
              >
                <VisibilityOutlinedIcon sx={{ fontSize: 16 }} />
              </Box>

              {/* Delete button */}
              <Box
                onClick={params.row.is_owner ? handleDelete : undefined}
                sx={{
                  display: "inline-flex",
                  alignItems: "center",
                  justifyContent: "center",
                  width: 32,
                  height: 32,
                  borderRadius: "8px",
                  backgroundColor: params.row.is_owner
                    ? "rgba(248,113,113,0.15)"
                    : "rgba(255,255,255,0.05)",
                  border: `1px solid ${params.row.is_owner ? "rgba(248,113,113,0.4)" : "rgba(255,255,255,0.12)"}`,
                  cursor: params.row.is_owner ? "pointer" : "not-allowed",
                  opacity: params.row.is_owner ? 1 : 0.5,
                  transition: "all 0.2s ease",
                  color: params.row.is_owner ? "#F87171" : "#94A3B8",
                  "&:hover": params.row.is_owner
                    ? {
                        backgroundColor: "rgba(248,113,113,0.25)",
                        transform: "scale(1.1) rotate(5deg)",
                        boxShadow: "0 4px 6px rgba(248, 113, 113, 0.2)",
                      }
                    : {},
                  "&:active": params.row.is_owner
                    ? {
                        transform: "scale(0.95)",
                      }
                    : {},
                }}
              >
                <DeleteIcon sx={{ fontSize: 16 }} />
              </Box>
            </Box>
          );
        },
      },
    ];
  }, [t, getIconFileType, dispatch, deleteFileWithId, page, entries, query]);

  return { dataSource, ref, previewFile, setPreviewFile };
};
