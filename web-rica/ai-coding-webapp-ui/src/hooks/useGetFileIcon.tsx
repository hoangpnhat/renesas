/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { useCallback } from "react";
import {
  Image as ImageIcon,
  InsertDriveFile as FileIcon,
} from "@mui/icons-material";
import { DocumentIcon, PDFIcon } from "../components/Icons.tsx";
import {
  CsvIcon,
  ExcelIcon,
  HTMLIcon,
  JsonIcon,
  MdIcons,
  TxtIcon,
} from "../components/icons/DocumentIcons.tsx";

export const useGetFileIcon = () => {
  return useCallback((fileType: string) => {
    switch (fileType) {
      case "image/png":
      case "image/jpeg":
      case "jpeg":
      case "png":
        return <ImageIcon />;
      case "application/pdf":
      case "pdf":
        return <PDFIcon />;
      case "text/markdown":
      case "md":
        return <MdIcons />;
      case "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet":
      case "xlsx":
        return <ExcelIcon />;
      case "text/csv":
      case "csv":
        return <CsvIcon />;
      case "docx":
      case "application/vnd.openxmlformats-officedocument.wordprocessingml.document":
        return <DocumentIcon />;
      case "text/html":
      case "html":
        return <HTMLIcon />;
      case "text/plain":
      case "txt":
        return <TxtIcon />;
      case "application/json":
        return <JsonIcon />;
      default:
        return <FileIcon />;
    }
  }, []);
};
