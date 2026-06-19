/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import * as yup from "yup";
import { ObjectSchema } from "yup";
import { useMemo } from "react";
import {
  KnowledgeBaseCreateOrEdit,
  useGetKnowledgeBaseDetailQuery
} from "../../../store/api-slices/knowledgeBaseAPISlice.ts";
//
export const useKnowledgeBaseSchema = (
  knowledgeBaseId?: string,
): {
  schema: ObjectSchema<KnowledgeBaseCreateOrEdit<string[]>>;
  isLoading: boolean;
} => {
  const { data: knowledgeDetail, isLoading } = useGetKnowledgeBaseDetailQuery(
    knowledgeBaseId!,
    {
      skip: !knowledgeBaseId,
    },
  );

  return {
    schema: useMemo(() => {
      return yup.object().shape({
        name: yup
          .string()
          .default(knowledgeDetail?.name || "")
          .required("Name is required"),
        description: yup
          .string()
          .default(knowledgeDetail?.description || "")
          .optional(),
        files: yup
          .array()
          // .of(yup.string())
          .default(knowledgeDetail?.files || []) // Extracts file_id
          .optional(),
      });
    }, [knowledgeDetail]) as unknown as ObjectSchema<
      KnowledgeBaseCreateOrEdit<string[]>
    >,
    isLoading,
  };
};
