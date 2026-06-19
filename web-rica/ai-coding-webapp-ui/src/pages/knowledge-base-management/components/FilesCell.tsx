import ArrowDropDown from "@mui/icons-material/ArrowDropDown";
import ArticleOutlinedIcon from "@mui/icons-material/ArticleOutlined";
import FiberManualRecordIcon from "@mui/icons-material/FiberManualRecord";
import { Popover, Box, Typography } from "@mui/material";
import React, { useState } from "react";
import { SimpleTreeView } from "@mui/x-tree-view/SimpleTreeView";
import { TreeItem } from "@mui/x-tree-view/TreeItem";
import dayjs from "dayjs";
import { File } from "../../../typings/component.props.ts";

export const FilesCell: React.FC<{ files: File[] }> = ({ files }) => {
  const [anchorEl, setAnchorEl] = useState<HTMLElement | null>(null);

  const handleClick = (event: React.MouseEvent<HTMLElement>) => {
    event.stopPropagation();
    setAnchorEl(event.currentTarget);
  };

  const handleClose = () => {
    setAnchorEl(null);
  };

  const open = Boolean(anchorEl);

  return (
    <Box
      sx={{
        display: "flex",
        alignItems: "center",
        height: "100%",
      }}
    >
      <Box
        onClick={handleClick}
        sx={{
          display: "inline-flex",
          alignItems: "center",
          gap: 0.75,
          padding: "4px 12px",
          borderRadius: "16px",
          backgroundColor: "#ede9fe",
          border: "1px solid #c4b5fd",
          height: "24px",
          cursor: "pointer",
          "&:hover": {
            backgroundColor: "#ddd6fe",
          },
        }}
      >
        <ArticleOutlinedIcon
          sx={{
            fontSize: 14,
            color: "#6b21a8",
          }}
        />
        <Typography
          sx={{
            fontSize: "12px",
            fontWeight: 600,
            color: "#6b21a8",
            lineHeight: 1,
          }}
        >
          {files.length}
        </Typography>
        <ArrowDropDown
          sx={{
            fontSize: 14,
            color: "#6b21a8",
          }}
        />
      </Box>
      <Popover
        open={open}
        anchorEl={anchorEl}
        onClose={handleClose}
        anchorOrigin={{
          vertical: "bottom",
          horizontal: "left",
        }}
        transformOrigin={{
          vertical: "top",
          horizontal: "left",
        }}
        slotProps={{
          paper: {
            sx: {
              mt: 1,
              p: 1,
              maxHeight: 200,
              minWidth: 250,
              overflow: "auto",
            },
          },
        }}
      >
        {files.length > 0 ? (
          <SimpleTreeView defaultExpandedItems={["files-root"]}>
            {files.map((file) => (
              <TreeItem
                key={file.file_id}
                itemId={file.file_id}
                label={
                  <Box
                    sx={{
                      display: "flex",
                      alignItems: "center",
                      gap: 1,
                      py: 0.5,
                    }}
                  >
                    <FiberManualRecordIcon sx={{ fontSize: 8, color: "#6b21a8" }} />
                    <Typography
                      sx={{
                        fontSize: "13px",
                        color: "#1e293b",
                        flex: 1,
                        overflow: "hidden",
                        textOverflow: "ellipsis",
                        whiteSpace: "nowrap",
                      }}
                    >
                      {file.file_name}
                    </Typography>
                    <Typography
                      sx={{
                        fontSize: "12px",
                        color: "#64748b",
                        whiteSpace: "nowrap",
                      }}
                    >
                      {dayjs(file.added_at).fromNow()}
                    </Typography>
                  </Box>
                }
              />
            ))}
          </SimpleTreeView>
        ) : (
          <Typography sx={{ fontSize: "13px", color: "#64748b", p: 1 }}>
            No files
          </Typography>
        )}
      </Popover>
    </Box>
  );
};
