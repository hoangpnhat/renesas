import { memo, useMemo, useState } from "react";
import Box from "@mui/material/Box";
import IconButton from "@mui/material/IconButton";
import Tooltip from "@mui/material/Tooltip";
import Typography from "@mui/material/Typography";
import CodeIcon from "@mui/icons-material/Code";
import VisibilityIcon from "@mui/icons-material/Visibility";
import ReactMarkdown from "react-markdown";
import remarkGfm from "remark-gfm";
import type { SyntaxHighlighterProps } from "react-syntax-highlighter";
import { PrismAsync } from "react-syntax-highlighter";
import oneDark from "react-syntax-highlighter/dist/esm/styles/prism/one-dark";
import ContentCopyIcon from "@mui/icons-material/ContentCopy";
import CheckIcon from "@mui/icons-material/Check";
import DownloadIcon from "@mui/icons-material/Download";
import { useTranslation } from "react-i18next";

const SyntaxHighlighter =
  PrismAsync as React.ComponentType<SyntaxHighlighterProps>;

type ViewMode = "code" | "preview";

interface ArtifactViewerProps {
  content: string;
  fileExtension: string;
  fileName?: string;
  maxHeight?: string;
  onDownload?: () => void;
}

export const ArtifactViewer = memo<ArtifactViewerProps>(
  ({ content, fileExtension, fileName, maxHeight = "400px", onDownload }) => {
    const { t } = useTranslation();
    const [viewMode, setViewMode] = useState<ViewMode>("code");
    const [copied, setCopied] = useState(false);

    const isMarkdown = fileExtension === "md" || fileExtension === "markdown";

    const syntaxLanguage = useMemo(() => {
      if (isMarkdown) return "markdown";
      if (fileExtension === "yaml" || fileExtension === "yml") return "yaml";
      return "text";
    }, [fileExtension, isMarkdown]);

    const handleCopy = async () => {
      try {
        await navigator.clipboard.writeText(content);
        setCopied(true);
        setTimeout(() => setCopied(false), 2000);
      } catch {
        const textarea = document.createElement("textarea");
        textarea.value = content;
        textarea.style.position = "fixed";
        textarea.style.left = "-999999px";
        document.body.appendChild(textarea);
        textarea.focus();
        textarea.select();
        document.execCommand("copy");
        document.body.removeChild(textarea);
        setCopied(true);
        setTimeout(() => setCopied(false), 2000);
      }
    };

    return (
      <Box
        sx={{
          border: "1px solid",
          borderColor: "divider",
          borderRadius: 2,
          overflow: "hidden",
        }}
      >
        {/* Header bar */}
        <Box
          sx={{
            display: "flex",
            alignItems: "center",
            justifyContent: "space-between",
            px: 1.5,
            py: 0.75,
            bgcolor: "#1E293B",
            borderBottom: "1px solid",
            borderBottomColor: "divider",
          }}
        >
          <Typography
            sx={{
              fontSize: "12px",
              color: "text.secondary",
              fontFamily: "monospace",
            }}
          >
            {fileName || `artifact.${fileExtension}`}
          </Typography>

          <Box sx={{ display: "flex", gap: 0.5 }}>
            {/* Copy button */}
            <Tooltip title={copied ? t("Copied!") : t("text.copy")} arrow>
              <IconButton
                size="small"
                onClick={handleCopy}
                sx={{ color: "#94a3b8", p: 0.5 }}
              >
                {copied ? (
                  <CheckIcon sx={{ fontSize: 16, color: "#10B981" }} />
                ) : (
                  <ContentCopyIcon sx={{ fontSize: 16 }} />
                )}
              </IconButton>
            </Tooltip>

            {/* Download button */}
            {onDownload && (
              <Tooltip title={t("customize.download")} arrow>
                <IconButton
                  size="small"
                  onClick={onDownload}
                  sx={{ color: "#94a3b8", p: 0.5 }}
                >
                  <DownloadIcon sx={{ fontSize: 16 }} />
                </IconButton>
              </Tooltip>
            )}

            {/* Code view toggle */}
            <Tooltip title={t("customize.codeView")} arrow>
              <IconButton
                size="small"
                onClick={() => setViewMode("code")}
                sx={{
                  color: viewMode === "code" ? "#7875E8" : "#94A3B8",
                  p: 0.5,
                  bgcolor:
                    viewMode === "code" ? "rgba(91,89,211,0.2)" : "transparent",
                  borderRadius: 1,
                }}
              >
                <CodeIcon sx={{ fontSize: 16 }} />
              </IconButton>
            </Tooltip>

            {/* Preview toggle (only meaningful for markdown) */}
            <Tooltip title={t("customize.previewView")} arrow>
              <IconButton
                size="small"
                onClick={() => setViewMode("preview")}
                sx={{
                  color: viewMode === "preview" ? "#10B981" : "#94A3B8",
                  p: 0.5,
                  bgcolor:
                    viewMode === "preview"
                      ? "rgba(16,185,129,0.2)"
                      : "transparent",
                  borderRadius: 1,
                }}
              >
                <VisibilityIcon sx={{ fontSize: 16 }} />
              </IconButton>
            </Tooltip>
          </Box>
        </Box>

        {/* Content area */}
        {viewMode === "code" ? (
          <Box
            sx={{
              bgcolor: "background.default",
              maxHeight,
              overflow: "auto",
              "& pre": {
                margin: "0 !important",
                background: "transparent !important",
              },
            }}
          >
            <SyntaxHighlighter
              language={syntaxLanguage}
              style={oneDark}
              showLineNumbers
              wrapLines
              wrapLongLines
              customStyle={{
                borderRadius: 0,
                padding: "16px",
                margin: 0,
                fontSize: "13px",
                lineHeight: "1.5",
                background: "transparent",
                maxHeight,
                overflow: "auto",
              }}
              lineNumberStyle={{
                minWidth: "2.5em",
                paddingRight: "1em",
                textAlign: "right" as const,
                userSelect: "none",
                color: "#64748B",
                borderRight: "1px solid #334155",
                marginRight: "1em",
              }}
            >
              {content}
            </SyntaxHighlighter>
          </Box>
        ) : (
          <Box
            sx={{
              maxHeight,
              overflow: "auto",
              p: 2.5,
              bgcolor: "background.paper",
            }}
          >
            {isMarkdown ? (
              <Box
                sx={{
                  "& h1, & h2, & h3, & h4, & h5, & h6": {
                    fontWeight: 700,
                    mt: 2,
                    mb: 1,
                    color: "#F3F4F6",
                  },
                  "& h1": {
                    fontSize: "1.5rem",
                    borderBottom: "2px solid #334155",
                    pb: 1,
                  },
                  "& h2": {
                    fontSize: "1.25rem",
                    borderBottom: "1px solid #334155",
                    pb: 0.5,
                  },
                  "& h3": { fontSize: "1.1rem" },
                  "& p": {
                    mb: 1.5,
                    color: "#D1D5DB",
                    lineHeight: 1.7,
                    fontSize: "14px",
                  },
                  "& ul, & ol": { pl: 3, mb: 1.5 },
                  "& li": { mb: 0.5, color: "#D1D5DB", fontSize: "14px" },
                  "& code": {
                    bgcolor: "rgba(255,255,255,0.08)",
                    px: 0.75,
                    py: 0.25,
                    borderRadius: "4px",
                    fontFamily: "monospace",
                    fontSize: "12px",
                    color: "#F87171",
                  },
                  "& pre": {
                    bgcolor: "#1E293B",
                    borderRadius: 1,
                    p: 2,
                    overflow: "auto",
                    "& code": {
                      bgcolor: "transparent",
                      color: "#D1D5DB",
                      p: 0,
                    },
                  },
                  "& blockquote": {
                    borderLeft: "4px solid #475569",
                    pl: 2,
                    ml: 0,
                    color: "#94A3B8",
                    fontStyle: "italic",
                  },
                  "& table": {
                    width: "100%",
                    borderCollapse: "collapse",
                    mb: 2,
                    fontSize: "13px",
                  },
                  "& th": {
                    bgcolor: "#1E293B",
                    fontWeight: 600,
                    p: "8px 12px",
                    border: "1px solid #334155",
                    textAlign: "left",
                  },
                  "& td": { p: "6px 12px", border: "1px solid #334155" },
                  "& a": { color: "#7875E8", textDecoration: "underline" },
                  "& hr": {
                    border: "none",
                    borderTop: "1px solid #334155",
                    my: 2,
                  },
                  "& strong": { fontWeight: 700, color: "#F3F4F6" },
                }}
              >
                <ReactMarkdown remarkPlugins={[remarkGfm]}>
                  {content}
                </ReactMarkdown>
              </Box>
            ) : (
              // YAML / other: structured key-value display
              <Box>
                {content.split("\n").map((line, i) => {
                  const isComment = line.trimStart().startsWith("#");
                  const isSectionKey =
                    /^[a-zA-Z_][\w-]*:/.test(line) && !line.startsWith(" ");
                  const isNestedKey = /^\s+[a-zA-Z_][\w-]*:/.test(line);
                  const isListItem = /^\s*-\s/.test(line);

                  return (
                    <Box
                      key={i}
                      sx={{
                        fontFamily: "monospace",
                        fontSize: "13px",
                        lineHeight: "1.6",
                        whiteSpace: "pre",
                        color: isComment
                          ? "#64748B"
                          : isSectionKey
                            ? "#6BBFFF"
                            : isNestedKey
                              ? "#60A5FA"
                              : isListItem
                                ? "#D1D5DB"
                                : "#94A3B8",
                        fontWeight: isSectionKey ? 600 : "normal",
                        fontStyle: isComment ? "italic" : "normal",
                      }}
                    >
                      {line || "\u00a0"}
                    </Box>
                  );
                })}
              </Box>
            )}
          </Box>
        )}
      </Box>
    );
  },
);

ArtifactViewer.displayName = "ArtifactViewer";
