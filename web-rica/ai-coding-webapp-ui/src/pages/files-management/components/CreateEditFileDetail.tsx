/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import Box from "@mui/material/Box/Box";
import LinearProgress from "@mui/material/LinearProgress";
import React, { Fragment, useCallback, useMemo } from "react";
import { useDispatch, useSelector } from "react-redux";
import { CommonFormScreen } from "../../../components/form/CommonFormScreen.tsx";
import { RootState } from "../../../store";
import { updateDrawerConfig } from "../../../store/reducers/drawer.slice.ts";
import { resetValidation } from "../../../store/reducers/fileValidateStatus.slice.ts";
import { resetProgress } from "../../../store/reducers/progress.slice.ts";
import { ExternalSx } from "../../../typings/common.props.ts";
import { useFileSchema } from "../hooks/useFileSchema.tsx";
import { useFileScreenDatum } from "../hooks/useFileScreenDatum.tsx";
import {
  FileCreateOrEdit,
  FileManagement,
  useUploadFileMutation,
} from "../../../store/api-slices/fileApiSlice.ts";

interface CreateEditFileDetailProps extends ExternalSx {
  fileId?: string;
}

export const CreateEditFileDetail: React.FC<CreateEditFileDetailProps> = ({
  externalSX,
  fileId,
}) => {
  const progress = useSelector((state: RootState) => state.progress);
  const [uploadFile] = useUploadFileMutation();
  const { formItem } = useFileScreenDatum();
  const dispatch = useDispatch();
  const onClose = useCallback(() => {
    dispatch(updateDrawerConfig({ isOpen: false, children: null }));
    dispatch(resetProgress());
  }, [dispatch]);

  const { schema, isLoading } = useFileSchema(fileId);

  const isSubmitting = useMemo<boolean>(() => {
    for (const key in progress) {
      if (Object.prototype.hasOwnProperty.call(progress, key)) {
        if (progress[key].status === "pending") return true;
      }
    }

    return false;
  }, [progress]);

  const handleSubmit = useCallback(
    async (data: FileCreateOrEdit) => {
      // reset file validation state slice when submit upload file
      dispatch(resetValidation());

      try {
        const uploadPromises: Promise<FileManagement>[] = [];
        const prepareUpload = async () => {
          // Set formData to send to upload API
          const formData = new FormData();
          formData.set("file", data.file);

          return await uploadFile({
            bodyRequest: formData,
            dispatch,
          }).unwrap();
        };

        uploadPromises.push(prepareUpload());
        // for (const fileItem of data.file) {
        //   const prepareUpload = async () => {
        //     // Set formData to send to upload API
        //     const formData = new FormData();
        //     formData.set("file", fileItem);

        //     return await uploadFile({
        //       bodyRequest: formData,
        //       dispatch,
        //     }).unwrap();
        //   };

        //   uploadPromises.push(prepareUpload());
        // }

        await Promise.allSettled(uploadPromises);
      } catch (e) {
        console.error("Failed to upload file:", e);
      } finally {
        onClose();
      }
    },
    [onClose, uploadFile, dispatch],
  );

  return (
    <Fragment>
      <Box
        className="project-creation-section"
        sx={{ height: "100%", overflowY: "hidden", ...externalSX }}
      >
        {isLoading && <LinearProgress />}
        <CommonFormScreen
          isSubmitting={isSubmitting}
          externalSX={{
            display: "flex",
            paddingY: 1,
            flexDirection: "column",
            justifyContent: "start",
          }}
          schema={schema}
          onSubmit={handleSubmit}
          items={formItem}
          onCancel={onClose}
        />
      </Box>
    </Fragment>
  );
};
