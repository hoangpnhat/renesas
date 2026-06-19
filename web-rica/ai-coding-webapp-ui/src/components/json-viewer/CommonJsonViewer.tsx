import React, { memo, useCallback, useEffect, useState } from "react";
import Box from "@mui/material/Box";
import Paper from "@mui/material/Paper";
import Typography from "@mui/material/Typography";
import IconButton from "@mui/material/IconButton";
import TextField from "@mui/material/TextField";
import Button from "@mui/material/Button";
import Alert from "@mui/material/Alert";
import { useTheme } from "@mui/material/styles";
import ContentCopy from "@mui/icons-material/ContentCopy";
import Search from "@mui/icons-material/Search";
import Clear from "@mui/icons-material/Clear";
import Code from "@mui/icons-material/Code";
import DataObject from "@mui/icons-material/DataObject";
import CommonJsonNode from "./CommonJsonNode.tsx";

// Type definitions
type JsonValue = string | number | boolean | null | JsonObject | JsonArray;

interface JsonObject {
  [key: string]: JsonValue;
}

interface JsonArray extends Array<JsonValue> {}

interface JsonViewerProps {
  initialJson?: string;
  maxHeight?: number | string;
  className?: string;
  hasJsonInput?: boolean;
  hasTitle?: boolean;
  hasJsonStructure?: boolean;
}

// Memoized JsonNode component

// Main JsonViewer component
const CommonJsonViewer: React.FC<JsonViewerProps> = memo(
  ({
    initialJson = `{}`,
    maxHeight = "100vh",
    className,
    hasJsonInput = false,
    hasTitle = false,
    hasJsonStructure = true,
  }) => {
    const theme = useTheme();
    const [jsonInput, setJsonInput] = useState<string>(initialJson);
    const [parsedJson, setParsedJson] = useState<JsonValue | null>(null);
    const [error, setError] = useState<string>("");
    const [searchTerm, setSearchTerm] = useState<string>("");
    const [expandedPaths, setExpandedPaths] = useState<Set<string>>(
      new Set(["root"]),
    );

    const parseJson = useCallback(() => {
      try {
        const parsed: JsonValue = JSON.parse(jsonInput);
        setParsedJson(parsed);
        setError("");
      } catch (err) {
        const errorMessage =
          err instanceof Error ? err.message : "Unknown error";
        setError(`Invalid JSON: ${errorMessage}`);
        setParsedJson(null);
      }
    }, [jsonInput]);
    useEffect(() => {
      setJsonInput(initialJson);
    }, [initialJson]);
    const copyToClipboard = useCallback(async (text: string) => {
      try {
        await navigator.clipboard.writeText(text);
      } catch (err) {
        console.error("Failed to copy to clipboard:", err);
      }
    }, []);

    const toggleExpand = useCallback((path: string) => {
      setExpandedPaths((prev) => {
        const newExpanded = new Set(prev);
        if (newExpanded.has(path)) {
          newExpanded.delete(path);
        } else {
          newExpanded.add(path);
        }
        return newExpanded;
      });
    }, []);

    const handleInputChange = useCallback(
      (e: React.ChangeEvent<HTMLInputElement>) => {
        setJsonInput(e.target.value);
      },
      [],
    );

    const handleSearchChange = useCallback(
      (e: React.ChangeEvent<HTMLInputElement>) => {
        setSearchTerm(e.target.value);
      },
      [],
    );

    const clearInput = useCallback(() => {
      setJsonInput("");
      setParsedJson(null);
      setError("");
    }, []);

    const clearSearch = useCallback(() => {
      setSearchTerm("");
    }, []);

    const collapseAll = useCallback(() => {
      setExpandedPaths(new Set(["root"]));
    }, []);

    const expandAll = useCallback(() => {
      // console.log("=>(CommonJsonViewer.tsx:119) parsedJson", parsedJson);
      if (!parsedJson) return;

      const allPaths = new Set(["root"]);
      const addPaths = (obj: JsonValue, path = "root") => {
        if (typeof obj === "object" && obj !== null) {
          Object.keys(obj).forEach((key) => {
            const newPath = `${path}.${key}`;
            allPaths.add(newPath);
            addPaths(
              Array.isArray(obj)
                ? obj[Number(key)]
                : (obj as JsonObject)[key as string],
              newPath,
            );
          });
        }
      };
      addPaths(parsedJson);
      setExpandedPaths(allPaths);
    }, [parsedJson]);

    const copyFormattedJson = useCallback(() => {
      if (parsedJson) {
        copyToClipboard(JSON.stringify(parsedJson, null, 2));
      }
    }, [parsedJson, copyToClipboard]);

    // Auto-parse on mount
    React.useEffect(() => {
      parseJson();
    }, [parseJson]);

    return (
      <Box sx={{ maxWidth: "90vw", mx: "auto", p: 1 }} className={className}>
        {hasTitle && (
          <Typography
            variant="h4"
            gutterBottom
            sx={{ display: "flex", alignItems: "center", gap: 1 }}
          >
            <DataObject color="primary" />
            JSON Viewer
          </Typography>
        )}

        {hasJsonInput && (
          <Paper
            elevation={0}
            sx={{
              p: 1,
              mb: hasJsonStructure ? 3 : 0,
              boxShadow: "initial !important",
            }}
          >
            <Typography variant="h6" gutterBottom>
              JSON Input
            </Typography>
            <TextField
              fullWidth
              multiline
              rows={15}
              value={jsonInput}
              onChange={handleInputChange}
              placeholder="Paste your JSON here..."
              sx={{
                mb: 2,
                "& .MuiInputBase-root": {
                  fontFamily: "monospace",
                  fontSize: "0.9rem",
                },
              }}
            />
            {hasJsonStructure && (
              <>
                <Button
                  variant="contained"
                  onClick={parseJson}
                  startIcon={<Code />}
                  sx={{ mr: 2 }}
                >
                  Parse JSON
                </Button>
                <Button
                  variant="outlined"
                  onClick={clearInput}
                  startIcon={<Clear />}
                >
                  Clear
                </Button>
              </>
            )}
          </Paper>
        )}

        {error && (
          <Alert severity="error" sx={{ mb: 3 }}>
            {error}
          </Alert>
        )}

        {parsedJson && hasJsonStructure && (
          <Paper elevation={0} sx={{ p: 1, boxShadow: "initial !important" }}>
            <Box
              sx={{
                display: "flex",
                justifyContent: "space-between",
                alignItems: "center",
                mb: 2,
              }}
            >
              <Typography variant="h6">JSON Structure</Typography>
              <TextField
                size="small"
                placeholder="Search keys and values..."
                value={searchTerm}
                onChange={handleSearchChange}
                InputProps={{
                  startAdornment: (
                    <Search sx={{ mr: 1, color: "text.secondary" }} />
                  ),
                  endAdornment: searchTerm && (
                    <IconButton size="small" onClick={clearSearch}>
                      <Clear fontSize="small" />
                    </IconButton>
                  ),
                }}
                sx={{ minWidth: 250 }}
              />
            </Box>

            <Box
              sx={{
                border: 1,
                borderColor: "divider",
                borderRadius: 1,
                bgcolor: theme.palette.mode === "dark" ? "grey.900" : "grey.50",
                p: 2,
                maxHeight,
                overflow: "auto",
              }}
            >
              <CommonJsonNode
                data={parsedJson}
                expandedPaths={expandedPaths}
                searchTerm={searchTerm}
                onToggleExpand={toggleExpand}
                onCopyToClipboard={copyToClipboard}
                theme={theme}
              />
            </Box>

            <Box sx={{ mt: 2, display: "flex", gap: 1, flexWrap: "wrap" }}>
              <Button size="small" onClick={collapseAll} variant="outlined">
                Collapse All
              </Button>
              <Button size="small" onClick={expandAll} variant="outlined">
                Expand All
              </Button>
              <Button
                size="small"
                onClick={copyFormattedJson}
                startIcon={<ContentCopy />}
                variant="outlined"
              >
                Copy Formatted JSON
              </Button>
            </Box>
          </Paper>
        )}
      </Box>
    );
  },
);

CommonJsonViewer.displayName = "JsonViewer";

export default CommonJsonViewer;
export type { JsonViewerProps, JsonValue, JsonObject, JsonArray };
