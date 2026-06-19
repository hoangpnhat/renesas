import React, { memo, useCallback, useMemo } from "react";
import Box from "@mui/material/Box";
import Typography from "@mui/material/Typography";
import Tooltip from "@mui/material/Tooltip";
import IconButton from "@mui/material/IconButton";
import ContentCopy from "@mui/icons-material/ContentCopy";
import ExpandMore from "@mui/icons-material/ExpandMore";
import ChevronRight from "@mui/icons-material/ChevronRight";
import Chip from "@mui/material/Chip";
import Collapse from "@mui/material/Collapse";
import { JsonArray, JsonObject, JsonValue } from "./CommonJsonViewer.tsx";
import { Theme } from "@mui/material/styles";

interface JsonNodeProps {
  data: JsonValue;
  path?: string;
  level?: number;
  parentKey?: string;
  expandedPaths: Set<string>;
  searchTerm: string;
  onToggleExpand: (path: string) => void;
  onCopyToClipboard: (text: string) => void;
  theme: Theme;
}

// Utility functions
const getTypeColor = (value: JsonValue, theme: Theme): string => {
  if (value === null) return theme.palette.grey[500];
  if (typeof value === "string") return theme.palette.success.main;
  if (typeof value === "number") return theme.palette.info.main;
  if (typeof value === "boolean") return theme.palette.warning.main;
  if (Array.isArray(value)) return theme.palette.secondary.main;
  if (typeof value === "object") return theme.palette.primary.main;
  return theme.palette.text.primary;
};

const getTypeName = (value: JsonValue): string => {
  if (value === null) return "null";
  if (Array.isArray(value)) return "array";
  return typeof value;
};

const highlightSearch = (text: string, searchTerm: string): string => {
  if (!searchTerm || typeof text !== "string") return text;
  const regex = new RegExp(`(${searchTerm})`, "gi");
  return text.replace(regex, "<mark>$1</mark>");
};

const shouldShowItem = (
  key: string | number,
  value: JsonValue,
  searchTerm: string,
): boolean => {
  if (!searchTerm) return true;
  const searchLower = searchTerm.toLowerCase();
  const keyMatch = key && String(key).toLowerCase().includes(searchLower);
  const valueMatch =
    typeof value === "string" && value.toLowerCase().includes(searchLower);
  return keyMatch || valueMatch;
};
const CommonJsonNode = memo<JsonNodeProps>(
  ({
    data,
    path = "root",
    level = 0,
    parentKey = "",
    expandedPaths,
    searchTerm,
    onToggleExpand,
    onCopyToClipboard,
    theme,
  }) => {
    const currentPath = `${path}.${parentKey}`;
    const isExpanded = expandedPaths.has(currentPath);
    const isObject =
      typeof data === "object" && data !== null && !Array.isArray(data);
    const isArray = Array.isArray(data);
    const isExpandable = isObject || isArray;

    const handleToggleExpand = useCallback(() => {
      onToggleExpand(currentPath);
    }, [currentPath, onToggleExpand]);

    const handleCopyValue = useCallback(() => {
      const stringValue = typeof data === "string" ? `"${data}"` : String(data);
      onCopyToClipboard(stringValue);
    }, [data, onCopyToClipboard]);

    const handleCopyObject = useCallback(
      (e: React.MouseEvent) => {
        e.stopPropagation();
        onCopyToClipboard(JSON.stringify(data, null, 2));
      },
      [data, onCopyToClipboard],
    );

    // Memoize entries calculation for complex objects/arrays
    const entries = useMemo(() => {
      if (!isExpandable) return [];
      return isArray
        ? (data as JsonArray).map(
            (item, index) => [index, item] as [number, JsonValue],
          )
        : Object.entries(data as JsonObject);
    }, [data, isArray, isExpandable]);

    const filteredEntries = useMemo(() => {
      return entries.filter(([key, value]) =>
        shouldShowItem(key, value, searchTerm),
      );
    }, [entries, searchTerm]);

    const itemCount = useMemo(() => {
      if (!data || (!isObject && !isArray)) return 0;
      return isArray
        ? (data as JsonArray).length
        : Object.keys(data as JsonObject).length;
    }, [data, isArray]);

    if (!isExpandable) {
      const stringValue = typeof data === "string" ? `"${data}"` : String(data);

      return (
        <Box
          sx={{
            ml: level * 3,
            py: 0.5,
            display: "flex",
            alignItems: "center",
            fontFamily: "monospace",
          }}
        >
          {parentKey && (
            <Typography
              component="span"
              sx={{ color: theme.palette.text.secondary, mr: 1 }}
            >
              "{parentKey}":
            </Typography>
          )}
          <Typography
            component="span"
            sx={{
              color: getTypeColor(data, theme),
              wordBreak: "break-word",
            }}
            dangerouslySetInnerHTML={{
              __html: highlightSearch(stringValue, searchTerm),
            }}
          />
          <Tooltip title={`Copy ${getTypeName(data)} value`}>
            <IconButton
              size="small"
              onClick={handleCopyValue}
              sx={{ ml: 1, opacity: 0.6 }}
            >
              <ContentCopy fontSize="inherit" />
            </IconButton>
          </Tooltip>
        </Box>
      );
    }

    return (
      <Box>
        <Box
          sx={{
            ml: level * 3,
            py: 0.5,
            display: "flex",
            alignItems: "center",
            cursor: "pointer",
            "&:hover": { bgcolor: theme.palette.action.hover },
            borderRadius: 1,
          }}
          onClick={handleToggleExpand}
        >
          <IconButton size="small">
            {isExpanded ? <ExpandMore /> : <ChevronRight />}
          </IconButton>

          {parentKey && (
            <Typography
              component="span"
              sx={{
                color: theme.palette.text.secondary,
                mr: 1,
                fontFamily: "monospace",
              }}
            >
              "{parentKey}":
            </Typography>
          )}

          <Box sx={{ display: "flex", alignItems: "center", gap: 1 }}>
            <Typography
              component="span"
              sx={{
                color: getTypeColor(data, theme),
                fontFamily: "monospace",
              }}
            >
              {isArray ? "[" : "{"}
            </Typography>

            <Chip
              size="small"
              label={`${itemCount} ${itemCount === 1 ? "item" : "items"}`}
              sx={{
                height: 20,
                fontSize: "0.7rem",
                bgcolor:
                  theme.palette.mode === "dark"
                    ? theme.palette.grey[700]
                    : theme.palette.grey[200],
              }}
            />

            {!isExpanded && (
              <Typography
                component="span"
                sx={{
                  color: getTypeColor(data, theme),
                  fontFamily: "monospace",
                }}
              >
                {isArray ? "]" : "}"}
              </Typography>
            )}
          </Box>

          <Tooltip title={`Copy ${isArray ? "array" : "object"}`}>
            <IconButton
              size="small"
              onClick={handleCopyObject}
              sx={{ ml: 1, opacity: 0.6 }}
            >
              <ContentCopy fontSize="inherit" />
            </IconButton>
          </Tooltip>
        </Box>

        <Collapse in={isExpanded}>
          <Box>
            {filteredEntries.map(([key, value], index) => (
              <CommonJsonNode
                key={`${key}-${index}`}
                data={value}
                path={currentPath}
                level={level + 1}
                parentKey={String(key)}
                expandedPaths={expandedPaths}
                searchTerm={searchTerm}
                onToggleExpand={onToggleExpand}
                onCopyToClipboard={onCopyToClipboard}
                theme={theme}
              />
            ))}
            {isExpanded && (
              <Typography
                component="div"
                sx={{
                  ml: level * 3 + 6,
                  color: getTypeColor(data, theme),
                  fontFamily: "monospace",
                }}
              >
                {isArray ? "]" : "}"}
              </Typography>
            )}
          </Box>
        </Collapse>
      </Box>
    );
  },
);
export default CommonJsonNode;

CommonJsonNode.displayName = "CommonJsonNode";
