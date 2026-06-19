import { useEffect, useRef, useState } from "react";
import Box from "@mui/material/Box";
import Typography from "@mui/material/Typography";
import InputBase from "@mui/material/InputBase";
import IconButton from "@mui/material/IconButton";
import Tooltip from "@mui/material/Tooltip";
import CircularProgress from "@mui/material/CircularProgress";
import SearchIcon from "@mui/icons-material/Search";
import AddIcon from "@mui/icons-material/Add";
import GavelIcon from "@mui/icons-material/Gavel";
import AutoAwesomeIcon from "@mui/icons-material/AutoAwesome";
import { useTranslation } from "react-i18next";
import {
  CustomizeListItem,
  CustomizeType,
} from "../../../store/api-slices/promptApiSlice.ts";

interface CustomizeListPanelProps {
  type: CustomizeType;
  items: CustomizeListItem[];
  isFetching: boolean;
  selectedId: string | null;
  onSelect: (item: CustomizeListItem) => void;
  onSearch: (query: string) => void;
  onCreateOpen: () => void;
}

export const CustomizeListPanel = ({
  type,
  items,
  isFetching,
  selectedId,
  onSelect,
  onSearch,
  onCreateOpen,
}: CustomizeListPanelProps) => {
  const { t } = useTranslation();
  const [search, setSearch] = useState("");
  const debounceRef = useRef<ReturnType<typeof setTimeout> | null>(null);

  const handleSearchChange = (val: string) => {
    setSearch(val);
    if (debounceRef.current) clearTimeout(debounceRef.current);
    debounceRef.current = setTimeout(() => onSearch(val), 300);
  };

  // Reset search input when type changes
  useEffect(() => {
    setSearch("");
    onSearch("");
  }, [type]);

  return (
    <Box
      sx={{
        width: 300,
        flexShrink: 0,
        display: "flex",
        flexDirection: "column",
        height: "100%",
        borderRight: "1px solid",
        borderColor: "divider",
        bgcolor: "background.paper",
      }}
    >
      {/* Panel header */}
      <Box
        sx={{
          px: 2,
          pt: 2.5,
          pb: 1.5,
          borderBottom: "1px solid",
          borderBottomColor: "divider",
          display: "flex",
          alignItems: "center",
          justifyContent: "space-between",
        }}
      >
        <Typography
          sx={{
            fontWeight: 700,
            fontSize: "15px",
            color: "text.primary",
            textTransform: "uppercase",
          }}
        >
          {t(type === "rule" ? "customize.typeRule" : "customize.typePrompt")}s
        </Typography>
        <Box sx={{ display: "flex", alignItems: "center", gap: 0.5 }}>
          <Tooltip
            title={t(
              type === "rule"
                ? "customize.createRule"
                : "customize.createPrompt",
            )}
            arrow
          >
            <IconButton
              size="small"
              onClick={onCreateOpen}
              sx={{
                color: "text.secondary",
                "&:hover": { color: "text.primary", bgcolor: "action.hover" },
              }}
            >
              <AddIcon sx={{ fontSize: 20 }} />
            </IconButton>
          </Tooltip>
        </Box>
      </Box>

      {/* Search */}
      <Box
        sx={{
          px: 1.5,
          py: 1,
          borderBottom: "1px solid",
          borderBottomColor: "divider",
        }}
      >
        <Box
          sx={{
            display: "flex",
            alignItems: "center",
            gap: 0.75,
            bgcolor: "background.default",
            border: "1px solid",
            borderColor: "divider",
            borderRadius: "8px",
            px: 1.25,
            py: 0.5,
          }}
        >
          <SearchIcon
            sx={{ fontSize: 16, color: "text.disabled", flexShrink: 0 }}
          />
          <InputBase
            value={search}
            onChange={(e) => handleSearchChange(e.target.value)}
            placeholder={t("searchText") + "..."}
            sx={{ fontSize: "13px", flex: 1, "& input": { p: 0 } }}
          />
        </Box>
      </Box>

      {/* List */}
      <Box sx={{ flex: 1, overflow: "auto" }}>
        {isFetching ? (
          <Box sx={{ display: "flex", justifyContent: "center", pt: 4 }}>
            <CircularProgress size={24} />
          </Box>
        ) : items.length === 0 ? (
          <Box sx={{ px: 2, pt: 4, textAlign: "center" }}>
            <Typography sx={{ fontSize: "13px", color: "text.disabled" }}>
              {search ? t("customize.noResults") : t("customize.emptyList")}
            </Typography>
          </Box>
        ) : (
          <Box sx={{ pt: 0.5 }}>
            {items.map((item) => {
              const isSelected = item.id === selectedId;
              const isRule = item.type === "rule";
              return (
                <Box
                  key={item.id}
                  onClick={() => onSelect(item)}
                  sx={{
                    mx: 1,
                    my: 0.25,
                    px: 1.5,
                    py: 1.25,
                    borderRadius: "8px",
                    cursor: "pointer",
                    bgcolor: isSelected ? "action.selected" : "transparent",
                    border: "1px solid",
                    borderColor: isSelected ? "divider" : "transparent",
                    transition: "all 0.15s ease",
                    "&:hover": {
                      bgcolor: "action.hover",
                      borderColor: "divider",
                    },
                  }}
                >
                  <Box
                    sx={{
                      display: "flex",
                      alignItems: "center",
                      gap: 1.25,
                    }}
                  >
                    <Box
                      sx={{
                        width: 28,
                        height: 28,
                        borderRadius: "7px",
                        bgcolor: isRule
                          ? "rgba(59,159,255,0.15)"
                          : "rgba(91,89,211,0.15)",
                        color: isRule ? "#6BBFFF" : "#7875E8",
                        display: "flex",
                        alignItems: "center",
                        justifyContent: "center",
                        flexShrink: 0,
                        mt: 0.1,
                      }}
                    >
                      {isRule ? (
                        <GavelIcon sx={{ fontSize: 14 }} />
                      ) : (
                        <AutoAwesomeIcon sx={{ fontSize: 14 }} />
                      )}
                    </Box>
                    <Box sx={{ overflow: "hidden" }}>
                      <Typography
                        sx={{
                          fontSize: "13px",
                          fontWeight: isSelected ? 600 : 500,
                          color: "text.primary",
                          overflow: "hidden",
                          textOverflow: "ellipsis",
                          whiteSpace: "nowrap",
                        }}
                      >
                        {item.name}
                      </Typography>
                    </Box>
                  </Box>
                </Box>
              );
            })}
          </Box>
        )}
      </Box>
    </Box>
  );
};
