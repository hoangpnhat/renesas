import { useEffect, useState } from "react";
import Box from "@mui/material/Box";
import Typography from "@mui/material/Typography";
import IconButton from "@mui/material/IconButton";
import Menu from "@mui/material/Menu";
import MenuItem from "@mui/material/MenuItem";
import Divider from "@mui/material/Divider";
import Chip from "@mui/material/Chip";
import CircularProgress from "@mui/material/CircularProgress";
import MoreHorizIcon from "@mui/icons-material/MoreHoriz";
import EditIcon from "@mui/icons-material/Edit";
import ShareIcon from "@mui/icons-material/Share";
import DeleteIcon from "@mui/icons-material/Delete";
import GavelIcon from "@mui/icons-material/Gavel";
import AutoAwesomeIcon from "@mui/icons-material/AutoAwesome";
import PersonOutlineIcon from "@mui/icons-material/PersonOutline";
import CalendarTodayOutlinedIcon from "@mui/icons-material/CalendarTodayOutlined";
import { useDispatch } from "react-redux";
import { useTranslation } from "react-i18next";
import {
  CustomizeItem,
  useGetCustomizeFileContentQuery,
} from "../../../store/api-slices/promptApiSlice.ts";
import { updateDialogConfig } from "../../../store/reducers/dialogComponent.slice.ts";
import { ArtifactViewer } from "./ArtifactViewer.tsx";
import {
  COMMON_DISPLAY_FORMAT_WITH_HOUR_24,
  normalizeDateString,
} from "../../../utils/dayjs.ts";

interface CustomizeDetailPanelProps {
  item: CustomizeItem;
  onEdit: () => void;
  onShare: () => void;
  onDelete: () => Promise<void>;
}

export const CustomizeDetailPanel = ({
  item,
  onEdit,
  onShare,
  onDelete,
}: CustomizeDetailPanelProps) => {
  const { t } = useTranslation();
  const dispatch = useDispatch();
  const [menuAnchor, setMenuAnchor] = useState<null | HTMLElement>(null);
  const [fileContent, setFileContent] = useState<string | null>(null);

  const { data: fileBlob, isFetching: isFetchingContent } =
    useGetCustomizeFileContentQuery(item.id);

  useEffect(() => {
    if (fileBlob) {
      fileBlob.text().then(setFileContent);
    } else {
      setFileContent(null);
    }
  }, [fileBlob]);

  const isRule = item.type === "rule";

  const handleDownload = () => {
    if (!fileBlob) return;
    const url = URL.createObjectURL(fileBlob);
    const a = document.createElement("a");
    a.href = url;
    a.download = item.prompt_file_info.prompt_file_name;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
  };

  const handleDelete = () => {
    setMenuAnchor(null);
    dispatch(
      updateDialogConfig({
        isOpen: true,
        title: t("customize.deleteTitle"),
        fullWidth: false,
        hasConfirmButton: true,
        hasCancelButton: true,
        hasAction: true,
        children: <span>{t("customize.confirmDelete")}</span>,
        onSubmit: onDelete,
      }),
    );
  };

  const handleEdit = () => {
    setMenuAnchor(null);
    onEdit();
  };

  const handleShare = () => {
    setMenuAnchor(null);
    onShare();
  };

  return (
    <Box
      sx={{
        flex: 1,
        display: "flex",
        flexDirection: "column",
        height: "100%",
        overflow: "hidden",
        bgcolor: "background.default",
      }}
    >
      {/* Header */}
      <Box
        sx={{
          px: 3,
          pt: 3,
          pb: 2,
          bgcolor: "background.paper",
          borderBottom: "1px solid",
          borderBottomColor: "divider",
        }}
      >
        <Box
          sx={{
            display: "flex",
            alignItems: "flex-start",
            justifyContent: "space-between",
            mb: 2,
          }}
        >
          <Box sx={{ display: "flex", alignItems: "center", gap: 1.5 }}>
            <Box
              sx={{
                width: 36,
                height: 36,
                borderRadius: "10px",
                bgcolor: isRule
                  ? "rgba(59,159,255,0.15)"
                  : "rgba(91,89,211,0.15)",
                color: isRule ? "#6BBFFF" : "#7875E8",
                display: "flex",
                alignItems: "center",
                justifyContent: "center",
                flexShrink: 0,
              }}
            >
              {isRule ? (
                <GavelIcon sx={{ fontSize: 18 }} />
              ) : (
                <AutoAwesomeIcon sx={{ fontSize: 18 }} />
              )}
            </Box>
            <Box>
              <Typography
                sx={{
                  fontSize: "18px",
                  fontWeight: 700,
                  color: "text.primary",
                  lineHeight: 1.3,
                }}
              >
                {item.name}
              </Typography>
              <Chip
                label={t(
                  isRule ? "customize.typeRule" : "customize.typePrompt",
                )}
                size="small"
                sx={{
                  mt: 0.5,
                  height: 18,
                  fontSize: "10px",
                  fontWeight: 600,
                  bgcolor: isRule
                    ? "rgba(59,159,255,0.15)"
                    : "rgba(91,89,211,0.15)",
                  color: isRule ? "#6BBFFF" : "#7875E8",
                  border: `1px solid ${isRule ? "rgba(59,159,255,0.4)" : "rgba(91,89,211,0.4)"}`,
                  "& .MuiChip-label": { px: 1 },
                }}
              />
            </Box>
          </Box>

          {/* Actions menu */}
          <IconButton
            size="small"
            onClick={(e) => setMenuAnchor(e.currentTarget)}
            sx={{
              color: "text.secondary",
              "&:hover": { bgcolor: "action.hover" },
            }}
          >
            <MoreHorizIcon />
          </IconButton>
          <Menu
            anchorEl={menuAnchor}
            open={Boolean(menuAnchor)}
            onClose={() => setMenuAnchor(null)}
            transformOrigin={{ horizontal: "right", vertical: "top" }}
            anchorOrigin={{ horizontal: "right", vertical: "bottom" }}
            PaperProps={{
              sx: {
                minWidth: 160,
                boxShadow: "0 4px 20px rgba(0,0,0,0.5)",
                borderRadius: 2,
              },
            }}
          >
            {item.can_edit && (
              <MenuItem
                onClick={handleEdit}
                sx={{ gap: 1.5, fontSize: "13px" }}
              >
                <EditIcon sx={{ fontSize: 16, color: "#7875E8" }} />
                {t("customize.editTitle")}
              </MenuItem>
            )}
            {item.can_share && (
              <MenuItem
                onClick={handleShare}
                sx={{ gap: 1.5, fontSize: "13px" }}
              >
                <ShareIcon sx={{ fontSize: 16, color: "#10B981" }} />
                {t("customize.shareTitle")}
              </MenuItem>
            )}
            {(item.can_edit || item.can_share) && item.can_delete && (
              <Divider />
            )}
            {item.can_delete && (
              <MenuItem
                onClick={handleDelete}
                sx={{ gap: 1.5, fontSize: "13px", color: "#F87171" }}
              >
                <DeleteIcon sx={{ fontSize: 16 }} />
                {t("customize.deleteTitle")}
              </MenuItem>
            )}
          </Menu>
        </Box>

        {/* Metadata row */}
        <Box sx={{ display: "flex", gap: 3 }}>
          <Box sx={{ display: "flex", alignItems: "center", gap: 0.75 }}>
            <PersonOutlineIcon sx={{ fontSize: 14, color: "#94a3b8" }} />
            <Typography sx={{ fontSize: "12px", color: "#64748b" }}>
              <Box component="span" sx={{ color: "#94a3b8", mr: 0.5 }}>
                {t("table.header.owner")}
              </Box>
              <Box
                component="span"
                sx={{ fontWeight: 600, color: "text.primary" }}
              >
                {item.owner_name}
              </Box>
            </Typography>
          </Box>
          {item.last_modified && (
            <Box sx={{ display: "flex", alignItems: "center", gap: 0.75 }}>
              <CalendarTodayOutlinedIcon
                sx={{ fontSize: 14, color: "#94a3b8" }}
              />
              <Typography sx={{ fontSize: "12px", color: "#64748b" }}>
                {
                  normalizeDateString(
                    item.last_modified,
                    true,
                    "en",
                    COMMON_DISPLAY_FORMAT_WITH_HOUR_24,
                  )?.split(" ")[0]
                }
              </Typography>
            </Box>
          )}
        </Box>
      </Box>

      {/* Scrollable body */}
      <Box sx={{ flex: 1, overflow: "auto", px: 3, py: 2.5 }}>
        {/* Description */}
        <Box sx={{ mb: 3 }}>
          <Typography
            sx={{
              fontSize: "11px",
              fontWeight: 600,
              color: "#94a3b8",
              mb: 0.75,
              textTransform: "uppercase",
              letterSpacing: "0.06em",
            }}
          >
            {t("customize.descriptionLabel")}
          </Typography>
          <Typography
            sx={{ fontSize: "14px", color: "text.secondary", lineHeight: 1.6 }}
          >
            {item.description}
          </Typography>
        </Box>

        <Divider sx={{ mb: 3 }} />

        {/* Artifact */}
        {isFetchingContent ? (
          <Box sx={{ display: "flex", justifyContent: "center", pt: 4 }}>
            <CircularProgress size={24} />
          </Box>
        ) : fileContent ? (
          <Box>
            <Typography
              sx={{
                fontSize: "11px",
                fontWeight: 600,
                color: "#94a3b8",
                mb: 1.5,
                textTransform: "uppercase",
                letterSpacing: "0.06em",
              }}
            >
              {t("customize.artifactPreview")}
            </Typography>
            <ArtifactViewer
              content={fileContent}
              fileExtension={item.prompt_file_info.prompt_file_extension}
              fileName={item.prompt_file_info.prompt_file_name}
              maxHeight="calc(100vh - 420px)"
              onDownload={handleDownload}
            />
          </Box>
        ) : (
          <Box
            sx={{
              border: "1px dashed",
              borderColor: "divider",
              borderRadius: 2,
              p: 4,
              textAlign: "center",
              color: "#94a3b8",
            }}
          >
            <Typography sx={{ fontSize: "13px" }}>
              {t("customize.noArtifact")}
            </Typography>
          </Box>
        )}
      </Box>
    </Box>
  );
};
