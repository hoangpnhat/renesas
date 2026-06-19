/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import React, { ChangeEvent, DragEvent, useRef, useState } from "react";

import Box from "@mui/material/Box";
import Button from "@mui/material/Button";
import LinearProgress from "@mui/material/LinearProgress";
import Typography from "@mui/material/Typography";
import CloudUploadIcon from "@mui/icons-material/CloudUpload";
import { styled } from "@mui/material/styles";

// Styled component for the invisible input
const VisuallyHiddenInput = styled("input")({
  clip: "rect(0 0 0 0)",
  clipPath: "inset(50%)",
  height: 1,
  overflow: "hidden",
  position: "absolute",
  bottom: 0,
  left: 0,
  whiteSpace: "nowrap",
  width: 1,
});

interface FileUploadProps {
  maxFileSize?: number; // in bytes
  acceptedFileTypes?: string[];
  onFileUploaded?: (file: File) => void;
}

const FileUpload: React.FC<FileUploadProps> = ({
  maxFileSize = 5 * 1024 * 1024, // 5MB default
  acceptedFileTypes = [],
  onFileUploaded,
}) => {
  const [selectedFile, setSelectedFile] = useState<File | null>(null);
  const [uploading, setUploading] = useState<boolean>(false);
  const [progress, setProgress] = useState<number>(0);
  const [dragActive, setDragActive] = useState<boolean>(false);
  const [error, setError] = useState<string | null>(null);
  const inputRef = useRef<HTMLInputElement>(null);

  const validateFile = (file: File): boolean => {
    setError(null);

    // Check file size
    if (file.size > maxFileSize) {
      setError(
        `File quá lớn. Kích thước tối đa: ${maxFileSize / (1024 * 1024)}MB`,
      );
      return false;
    }

    // Check file type if specified
    if (acceptedFileTypes.length > 0) {
      const fileType = file.type;
      if (!acceptedFileTypes.includes(fileType)) {
        setError(
          `Loại file không được hỗ trợ. Chấp nhận: ${acceptedFileTypes.join(", ")}`,
        );
        return false;
      }
    }

    return true;
  };

  const handleFileChange = (event: ChangeEvent<HTMLInputElement>) => {
    if (event.target.files && event.target.files[0]) {
      const file = event.target.files[0];
      if (validateFile(file)) {
        setSelectedFile(file);
      }
    }
  };

  const handleDrag = (e: DragEvent<HTMLDivElement>) => {
    e.preventDefault();
    e.stopPropagation();

    if (e.type === "dragenter" || e.type === "dragover") {
      setDragActive(true);
    } else if (e.type === "dragleave") {
      setDragActive(false);
    }
  };

  const handleDrop = (e: DragEvent<HTMLDivElement>) => {
    e.preventDefault();
    e.stopPropagation();
    setDragActive(false);

    if (e.dataTransfer.files && e.dataTransfer.files[0]) {
      const file = e.dataTransfer.files[0];
      if (validateFile(file)) {
        setSelectedFile(file);
      }
    }
  };

  const handleUpload = (): void => {
    if (!selectedFile) return;

    setUploading(true);

    // Simulate upload progress
    const timer = setInterval(() => {
      setProgress((prevProgress) => {
        if (prevProgress >= 100) {
          clearInterval(timer);
          setUploading(false);
          if (onFileUploaded && selectedFile) {
            onFileUploaded(selectedFile);
          }
          return 100;
        }
        return prevProgress + 10;
      });
    }, 500);
  };

  const getAcceptAttribute = (): string => {
    return acceptedFileTypes.join(",");
  };

  return (
    <Box sx={{ width: "100%", maxWidth: 500, p: 3 }}>
      <Box
        sx={{
          border: "2px dashed",
          borderColor: dragActive
            ? "primary.main"
            : error
              ? "error.main"
              : "#ccc",
          borderRadius: 2,
          p: 3,
          mb: 2,
          textAlign: "center",
          backgroundColor: dragActive
            ? "rgba(25, 118, 210, 0.04)"
            : error
              ? "rgba(211, 47, 47, 0.04)"
              : "background.elevated",
          cursor: "pointer",
          transition: "all 0.2s ease",
          "&:hover": {
            backgroundColor: error
              ? "rgba(211, 47, 47, 0.04)"
              : "rgba(25, 118, 210, 0.04)",
            borderColor: error ? "error.main" : "primary.light",
          },
        }}
        onDragEnter={handleDrag}
        onDragLeave={handleDrag}
        onDragOver={handleDrag}
        onDrop={handleDrop}
        onClick={() => inputRef.current && inputRef.current.click()}
      >
        <CloudUploadIcon
          sx={{
            fontSize: 48,
            color: error ? "error.main" : "primary.main",
            mb: 2,
          }}
        />

        <Typography variant="h6" gutterBottom>
          Kéo thả file hoặc nhấp để tải lên
        </Typography>

        <Typography variant="body2" color="text.secondary">
          {selectedFile
            ? `Đã chọn: ${selectedFile.name}`
            : acceptedFileTypes.length > 0
              ? `Hỗ trợ: ${acceptedFileTypes.join(", ")}`
              : "Hỗ trợ tất cả các định dạng file"}
        </Typography>

        {error && (
          <Typography variant="body2" color="error" sx={{ mt: 1 }}>
            {error}
          </Typography>
        )}

        <VisuallyHiddenInput
          type="file"
          ref={inputRef}
          onChange={handleFileChange}
          accept={getAcceptAttribute()}
        />
      </Box>

      {selectedFile && !error && (
        <>
          <Button
            variant="contained"
            color="primary"
            onClick={handleUpload}
            disabled={uploading}
            fullWidth
            startIcon={<CloudUploadIcon />}
          >
            {uploading ? "Đang tải lên..." : "Tải file lên"}
          </Button>

          {uploading && (
            <Box sx={{ mt: 2 }}>
              <LinearProgress variant="determinate" value={progress} />
              <Typography
                variant="body2"
                color="text.secondary"
                align="center"
                sx={{ mt: 1 }}
              >
                {`${Math.round(progress)}%`}
              </Typography>
            </Box>
          )}
        </>
      )}
    </Box>
  );
};

export default FileUpload;
