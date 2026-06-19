import { useMemo } from "react";
import * as yup from "yup";
import { ObjectSchema } from "yup";
import { MAX_FILE_SIZE } from "../../../constants/fileSize.ts";
import {
  FileCreateOrEdit,
  useGetFileDetailQuery,
} from "../../../store/api-slices/fileApiSlice.ts";

// Allowed file types
const VALID_FILE_TYPES = [
  "application/pdf",
  "application/vnd.openxmlformats-officedocument.presentationml.presentation",
  "application/vnd.ms-powerpoint",
  "application/vnd.openxmlformats-officedocument.wordprocessingml.document",
  "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
  "application/msword",
];

export const useFileSchema = (
  projectId?: string,
): { schema: ObjectSchema<FileCreateOrEdit>; isLoading: boolean } => {
  const { data: fileDetail, isLoading } = useGetFileDetailQuery(projectId!, {
    skip: !projectId,
  });

  return {
    schema: useMemo(
      () =>
        yup.object().shape({
          file: yup
            .mixed<File>()
            .required("File is required")
            .test(
              "fileType",
              "Only accept files PDF, PPT, XLSX, DOCX and DOC",
              (value) => {
                if (!value) return false;
                return VALID_FILE_TYPES.includes(value.type);
              },
            )
            .test(
              "fileSize",
              "File size must be greater than 0B and less than 20MB",
              (value: File) => {
                if (!value) return false;
                return value.size <= MAX_FILE_SIZE && value.size > 0;
              },
            ),
        }),
      [fileDetail],
    ) as ObjectSchema<FileCreateOrEdit>,
    isLoading,
  };
};
