import {
  Close as CloseIcon,
  CloudUpload as CloudUploadIcon,
} from "@mui/icons-material";
import {
  Button,
  IconButton,
  ListItem,
  ListItemIcon,
  ListItemText,
  Paper,
  Typography,
} from "@mui/material";
import { filesize } from "filesize";
import orderBy from "lodash/orderBy";
import React, { Fragment, useCallback, useState } from "react";
import { Controller, FieldError } from "react-hook-form";
import { useDispatch, useSelector } from "react-redux";
import { ACCEPT_FILE_FORMAT_UPLOAD } from "../../constants/context.ts";
import { useGetFileIcon } from "../../hooks/useGetFileIcon.tsx";
import { RootState } from "../../store";
import {
  removeValidationWithDocumentId,
  resetValidation,
} from "../../store/reducers/fileValidateStatus.slice.ts";
import { ListItemAction } from "../icons/ListItemAction.tsx";

interface CommonUploadFileProps {
  control: any;
  name: string;
  errors?: FieldError;
  helperText?: string;
  onFileChange?: (files: File | File[] | undefined) => void;
  customAccept?: string;
  additionalErrorMessage?: string;
  isSubmitting?: boolean;
  isMultiple?: boolean;
  validateFileSize?: number;
}

export const CommonUploadFile: React.FC<CommonUploadFileProps> = ({
  control,
  name,
  errors,
  helperText,
  onFileChange,
  validateFileSize,
  additionalErrorMessage = "",
  isMultiple,
  customAccept = ACCEPT_FILE_FORMAT_UPLOAD,
}) => {
  const dispatch = useDispatch();
  const [isDragging, setIsDragging] = useState<boolean>(false);
  const getFileIcon = useGetFileIcon();
  const progress = useSelector((state: RootState) => state.progress);
  const fileValidateStatus = useSelector(
    (state: RootState) => state.fileValidateStatus,
  );

  const handleFiles = useCallback(
    (files: FileList, onChange: (files: File[] | undefined) => void) => {
      onChange(Array.from(files)); // Update react-hook-form value
      dispatch(resetValidation());
      if (onFileChange) onFileChange(Array.from(files));
    },
    [dispatch, onFileChange],
  );

  // Remove file
  const removeFile = useCallback(
    (
      onChange: (files: File[] | undefined) => void,
      files: File[],
      fileItemName: string,
    ) => {
      const remainFiles = files.filter((file) => file.name !== fileItemName);

      dispatch(
        removeValidationWithDocumentId({
          documentId: fileItemName,
        }),
      );
      onChange(remainFiles.length ? remainFiles : undefined);
      if (onFileChange)
        onFileChange(remainFiles.length ? remainFiles : undefined);
    },
    [dispatch, onFileChange],
  );
  const handleFile = useCallback(
    (file: File, onChange: (file: File | undefined) => void) => {
      onChange(file); // Update react-hook-form value
      if (onFileChange) onFileChange(file); // Pass file outside
    },
    [onFileChange],
  );
  // Handle file drop
  const handleDrop = useCallback(
    (
      e: React.DragEvent,
      onChange: (files: File | File[] | undefined) => void,
    ) => {
      e.preventDefault();
      setIsDragging(false);
      const droppedFiles = e.dataTransfer.files;
      if (droppedFiles) {
        if (isMultiple) {
          handleFiles(droppedFiles, onChange);
        } else {
          handleFile(droppedFiles[0], onChange);
        }
      }
    },
    [handleFiles, handleFile, isMultiple],
  );
  const isError = !!errors;

  // Open file dialog
  const handleClick = useCallback((event: React.MouseEvent) => {
    event.stopPropagation();
    document.getElementById("file-input")?.click();
  }, []);

  // Handle file input change
  const handleInputChange = useCallback(
    (
      e: React.ChangeEvent<HTMLInputElement>,
      onChange: (files: File | File[] | undefined) => void,
    ) => {
      const selectedFiles = e.target.files || undefined;
      if (selectedFiles) {
        if (isMultiple) {
          handleFiles(selectedFiles, onChange);
        } else {
          handleFile(selectedFiles[0], onChange);
        }
      }
      e.target.value = ""; // Reset input
    },
    [handleFiles, handleFile, isMultiple],
  );

  const renderListOfFile = useCallback(
    (files: File | File[], onChange: (files: File[] | undefined) => void) => {
      const renderValue = orderBy(
        Array.isArray(files) ? files : [files],
        ["size"],
        ["desc"],
      );
      return renderValue.map((item: File, index: number) => (
        <Paper key={index} elevation={1} sx={{ mt: 1 }}>
          <ListItem
            sx={
              fileValidateStatus[item.name]?.status === "failed"
                ? {
                    border: "1px solid red",
                  }
                : {}
            }
            secondaryAction={
              <ListItemAction
                value={progress[item.name]?.loadingPercent ?? 0}
                submitStatus={progress[item.name]?.status ?? "pending"}
                progress={progress}
              >
                <IconButton
                  onClick={() => removeFile(onChange, renderValue, item.name)}
                >
                  <CloseIcon />
                </IconButton>
              </ListItemAction>
            }
          >
            <ListItemIcon>{getFileIcon(item.type)}</ListItemIcon>
            <ListItemText
              primary={item.name}
              secondary={
                <Typography variant="caption" color="textSecondary">
                  {filesize(item.size)}
                </Typography>
              }
            />
          </ListItem>
        </Paper>
      ));
    },
    [progress, fileValidateStatus, getFileIcon, removeFile],
  );
  // Format file size
  return (
    <Controller
      name={name}
      control={control}
      render={({ field: { value = undefined, onChange } }) => (
        <Fragment>
          {/* Dropzone */}
          <Paper
            sx={{
              border: "2px dashed",
              borderColor: isDragging
                ? "primary.main"
                : isError
                  ? "error.main"
                  : "divider",
              backgroundColor: isDragging
                ? "rgba(25, 118, 210, 0.04)"
                : "background.paper",
              p: 1,
              textAlign: "center",
              cursor: "pointer",
              transition: "all 0.2s ease-in-out",
              "&:hover": { borderColor: "primary.main" },
            }}
            onDragEnter={() => setIsDragging(true)}
            onDragLeave={() => setIsDragging(false)}
            onDragOver={(e) => e.preventDefault()}
            onDrop={(e) => handleDrop(e, onChange)}
            onClick={handleClick}
          >
            <CloudUploadIcon
              sx={{
                fontSize: 48,
                color: isDragging ? "primary.main" : "text.secondary",
                mb: 1,
              }}
            />
            <Typography variant="h6" gutterBottom>
              {isDragging ? "Drop file here" : "Drag & drop file here"}
            </Typography>

            <Typography variant="body2" color="textSecondary" gutterBottom>
              or
            </Typography>
            <Button
              variant="contained"
              color="primary"
              sx={{ mt: 1 }}
              onClick={handleClick}
            >
              Browse File
            </Button>
            <input
              id="file-input"
              type="file"
              accept={customAccept}
              hidden
              multiple={isMultiple}
              onChange={(e) => handleInputChange(e, onChange)}
            />
            <Typography
              variant="caption"
              color="textSecondary"
              display="block"
              sx={{ mt: 1 }}
            >
              {validateFileSize && (
                <Fragment>
                  {`Max file size: ${filesize(validateFileSize, {
                    base: 2,
                    round: 0,
                    standard: "iec",
                    symbols: { MiB: "MB" },
                  })}`}
                  <br />
                </Fragment>
              )}
              Only accept
              <strong>
                <i>{customAccept} </i>
              </strong>
              files
            </Typography>
            {helperText && (
              <Typography
                variant="caption"
                color={isError ? "error" : "textSecondary"}
                sx={{ mt: 1 }}
              >
                {errors?.["type"] === "fileContent" && additionalErrorMessage
                  ? additionalErrorMessage
                  : helperText}
              </Typography>
            )}
          </Paper>

          {/* Display selected file */}
          {value && renderListOfFile(value, onChange)}
        </Fragment>
      )}
    />
  );
};
