/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import React, { Fragment, useCallback } from "react";
import Box from "@mui/material/Box/Box";
import { ExternalSx } from "../../../typings/common.props.ts";
import { CommonFormScreen } from "../../../components/form/CommonFormScreen.tsx";
import { updateDrawerConfig } from "../../../store/reducers/drawer.slice.ts";
import LinearProgress from "@mui/material/LinearProgress";
import { useDispatch } from "react-redux";

import { useKnowledgeBaseDatum } from "../hooks/useKnowledgeBaseDatum.tsx";
import { useKnowledgeBaseSchema } from "../hooks/useKnowledgeBaseSchema.tsx";
import {
  FileJunction,
  KnowledgeBaseCreateOrEdit,
  useCreateKnowledgeBaseMutation,
  useLazyGetKnowledgeBasesQuery,
  useUpdateKnowledgeBaseMutation,
} from "../../../store/api-slices/knowledgeBaseAPISlice.ts";
import { FileManagement } from "../../../store/api-slices/fileApiSlice.ts";
import { ENTRIES_STANDARD_RECORDS } from "../../../constants/common.ts";

interface CreateEditKnowledgeDetailProps extends ExternalSx {
  knowledgeBaseId?: string;
  fileList: FileManagement[];
  currentUserRole?: string;
}

export const CreateEditKnowledgeBaseDetail: React.FC<
  CreateEditKnowledgeDetailProps
> = ({ externalSX, fileList, knowledgeBaseId, currentUserRole }) => {
  const [createKnowledgeBase, { isLoading: isCreatingKB }] =
    useCreateKnowledgeBaseMutation();
  const [updateKnowledgeBase, { isLoading: isUpdatingKB }] =
    useUpdateKnowledgeBaseMutation();
  const isEdit = !!knowledgeBaseId;

  const formItem = useKnowledgeBaseDatum({
    currentUserRole,
    isEdit,
    knowledgeBaseId,
  });
  const dispatch = useDispatch();
  const onClose = useCallback(() => {
    dispatch(updateDrawerConfig({ isOpen: false, children: null }));
  }, [dispatch]);

  // eslint-disable-next-line @typescript-eslint/no-unused-vars
  const [triggerGetKB, _] = useLazyGetKnowledgeBasesQuery();

  const handleRefetch = useCallback(async () => {
    await triggerGetKB({
      page: 0,
      entries: ENTRIES_STANDARD_RECORDS,
      query: "",
    });
  }, [triggerGetKB]);

  const { schema, isLoading } = useKnowledgeBaseSchema(knowledgeBaseId);
  const handleSubmit = useCallback(
    async (data: KnowledgeBaseCreateOrEdit<string[]>) => {
      try {
        const requestBody: KnowledgeBaseCreateOrEdit<FileJunction[]> = {
          name: (data["name"] as string) || "No Name",
          description: (data["description"] as string) || "",
          files: (data?.files || []).map((file: any) => ({
            file_id: typeof file === "string" ? file : file.file_id || file.id,
            file_name: typeof file === "string" ? "" : file.file_name || "",
            added_at:
              typeof file === "string"
                ? new Date().toISOString()
                : file.added_at || new Date().toISOString(),
          })),
        };

        if (!knowledgeBaseId) {
          await createKnowledgeBase(requestBody).unwrap();
        } else {
          await updateKnowledgeBase({
            id: knowledgeBaseId,
            requestBody: requestBody,
          });
        }
      } catch (e) {
        console.error("Failed to create project:", e);
      } finally {
        onClose();
        await handleRefetch();
      }
    },
    [
      fileList,
      knowledgeBaseId,
      createKnowledgeBase,
      updateKnowledgeBase,
      onClose,
      handleRefetch,
    ],
  );

  return (
    <Fragment>
      {/*{isLoading && <Box sx={{ width: "100%" }}></Box>}*/}
      <Box
        className="project-creation-section"
        sx={{
          height: "100%",
          overflowY: "hidden",
          ...externalSX,
        }}
      >
        {isLoading && <LinearProgress />}
        <CommonFormScreen
          externalSX={{
            display: "flex",
            paddingY: 1,
            flexDirection: "column",
            gap: 2,
            justifyContent: "start",
          }}
          isSubmitting={isCreatingKB || isUpdatingKB}
          schema={schema}
          onSubmit={handleSubmit}
          items={formItem}
          onCancel={onClose}
        />
      </Box>
    </Fragment>
  );
};
