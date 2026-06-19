/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { useCallback } from "react";
import { getFileNameFromURL, isURL } from "../utils/utilities";
import { GET_EXTENSION_FROM_URL } from "../constants/regex";
import {
  DocumentIcon,
  ExcelIcon,
  MediaIcon,
  PDFIcon,
  PowerPointIcon,
  WordIcon,
} from "../components/Icons";
import CheckCircleIcon from "@mui/icons-material/CheckCircle";
import FolderIcon from "@mui/icons-material/Folder";
import MovieIcon from "@mui/icons-material/Movie";
import { ImageFormatFileIcon } from "../components/icons/ImageFormatFileIcon";
import { ImageFormat } from "../components/icons/ImageFormatFileIcon.tsx";

export const useIconGetter = ({
  fontSize,
  className,
}: {
  fontSize: any;
  className?: string;
}) => {
  const getIconFileType = useCallback(
    (fileURI: string, isFolder: boolean = false) => {
      const fileType = getFileNameFromURL(fileURI, GET_EXTENSION_FROM_URL, 1);
      switch (fileType) {
        case "wmv":
          return <MediaIcon fontSize={fontSize} className={className} />;
        case "doc":
        case "docx":
          return <WordIcon fontSize={fontSize} className={className} />;
        case "pdf":
          return <PDFIcon fontSize={fontSize} className={className} />;
        case "ppt":
        case "pptx":
          return <PowerPointIcon fontSize={fontSize} className={className} />;
        case "xls":
        case "xlsx":
          return <ExcelIcon fontSize={fontSize} className={className} />;
        case "mp4":
        case "mov":
          return <MovieIcon fontSize={fontSize} className={className} />;
        case "jpg":
        case "jpeg":
        case "png":
        case "gif":
        case "bmp":
        case "webp":
          return (
            <ImageFormatFileIcon
              format={fileType as ImageFormat}
              className={className}
            />
          );
        default:
          return !isURL(fileURI) ? (
            isFolder ? (
              <FolderIcon htmlColor="#1976d2" className={className} />
            ) : (
              <CheckCircleIcon
                fontSize={fontSize}
                className={className}
                htmlColor="#28a745"
              />
            )
          ) : (
            <DocumentIcon fontSize={fontSize} className={className} />
          );
      }
    },
    [className, fontSize],
  );
  return { getIconFileType };
};
