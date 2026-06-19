import { memo, PropsWithChildren, useCallback, useState } from "react";

import IconButton from "@mui/material/IconButton";
import Tooltip from "@mui/material/Tooltip";
import Box from "@mui/material/Box";
import CheckIcon from "@mui/icons-material/Check";
import ContentCopyIcon from "@mui/icons-material/ContentCopy";
// Fix TypeScript issue bằng cách import như này
import { PrismAsync } from "react-syntax-highlighter";
import dracula from "react-syntax-highlighter/dist/esm/styles/prism/dracula";

interface CopyCodeBlockProps extends PropsWithChildren {
  className?: string;
  showLineNumbers?: boolean;
  maxHeight?: string;
  wrapLongLines?: boolean;
}

// Danh sách ngôn ngữ được hỗ trợ (để tối ưu bundle)
const SUPPORTED_LANGUAGES = new Set([
  "javascript",
  "js",
  "jsx",
  "typescript",
  "ts",
  "tsx",

  "html",
  "xml",
  "json",

  "shell",
  "sh",
  "python",
  "py",

  "c",
  "cpp",
  "c++",

  "markdown",
  "md",
  "dockerfile",
]);

const isLanguageSupported = (language: string): boolean => {
  return SUPPORTED_LANGUAGES.has(language.toLowerCase());
};

// Type assertion để fix TypeScript JSX issue
const SyntaxHighlighter = PrismAsync as React.ComponentType<any>;

export const CommonCopyCodeBlock = memo<CopyCodeBlockProps>(
  ({
    className = "",
    children,
    showLineNumbers = true,
    maxHeight = "500px",
    wrapLongLines = true,
  }) => {
    const [copied, setCopied] = useState(false);

    const match = /language-(\w+)/.exec(className);
    const rawLanguage = match ? match[1] : "text";

    // Normalize language và check support
    const language = isLanguageSupported(rawLanguage) ? rawLanguage : undefined;
    const codeContent = String(children).trim();

    const handleCopyCode = useCallback(async () => {
      try {
        await navigator.clipboard.writeText(codeContent);
        setCopied(true);
        setTimeout(() => setCopied(false), 2000);
      } catch (err) {
        console.error("Failed to copy code:", err);

        // Fallback cho các browser cũ
        try {
          const textArea = document.createElement("textarea");
          textArea.value = codeContent;
          textArea.style.position = "fixed";
          textArea.style.left = "-999999px";
          textArea.style.top = "-999999px";
          document.body.appendChild(textArea);
          textArea.focus();
          textArea.select();

          const successful = document.execCommand("copy");
          if (successful) {
            setCopied(true);
            setTimeout(() => setCopied(false), 2000);
          }

          document.body.removeChild(textArea);
        } catch (fallbackErr) {
          console.error("Fallback copy also failed:", fallbackErr);
        }
      }
    }, [codeContent]);

    return (
      <Box
        sx={{
          position: "relative",
          borderRadius: 2,
          overflow: "hidden",
          maxHeight,
          bgcolor: "#282a36", // Dracula background color
          "&:hover .copy-button": {
            opacity: 1,
          },
          "& pre": {
            margin: "0 !important",
            background: "transparent !important",
          },
        }}
      >
        <Tooltip
          title={copied ? "Copied!" : "Copy code"}
          placement="left"
          arrow
        >
          <IconButton
            className="copy-button"
            onClick={handleCopyCode}
            size="small"
            sx={{
              position: "absolute",
              top: 12,
              right: 12,
              zIndex: 10,
              opacity: 0,
              transition: "opacity 0.2s ease-in-out, transform 0.1s ease",
              backgroundColor: "rgba(255, 255, 255, 0.1)",
              backdropFilter: "blur(8px)",
              border: "1px solid rgba(255, 255, 255, 0.2)",
              color: "rgba(255, 255, 255, 0.8)",
              "&:hover": {
                backgroundColor: "rgba(255, 255, 255, 0.2)",
                transform: "scale(1.05)",
                color: "white",
              },
            }}
          >
            {copied ? (
              <CheckIcon
                sx={{
                  fontSize: 18,
                  color: "success.main",
                }}
              />
            ) : (
              <ContentCopyIcon
                sx={{
                  fontSize: 18,
                }}
              />
            )}
          </IconButton>
        </Tooltip>

        <SyntaxHighlighter
          language={language}
          style={dracula}
          showLineNumbers={showLineNumbers}
          wrapLines={true}
          wrapLongLines={wrapLongLines}
          customStyle={{
            borderRadius: "8px",
            paddingTop: "16px",
            paddingRight: "60px", // Extra space cho copy button
            paddingBottom: "16px",
            paddingLeft: "16px",
            margin: "0",
            fontSize: "14px",
            lineHeight: "1.5",
            background: "transparent",
            maxHeight: "inherit",
            overflow: "auto",
          }}
          lineNumberStyle={{
            minWidth: "3em",
            paddingRight: "1em",
            textAlign: "right" as const,
            userSelect: "none",
            color: "rgba(255, 255, 255, 0.4)",
            borderRight: "1px solid rgba(255, 255, 255, 0.1)",
            marginRight: "1em",
          }}
          codeTagProps={{
            style: {
              fontFamily:
                "'JetBrains Mono', 'Fira Code', 'Monaco', 'Consolas', monospace",
            },
          }}
        >
          {codeContent}
        </SyntaxHighlighter>
      </Box>
    );
  },
);

CommonCopyCodeBlock.displayName = "CopyCodeBlock";
