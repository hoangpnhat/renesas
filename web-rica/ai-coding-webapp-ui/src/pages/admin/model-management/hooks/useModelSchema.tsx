/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import * as yup from "yup";
import { ObjectSchema } from "yup";
import { useMemo } from "react";

import {
  ModelCreateOrEdit,
  useGetModelsByIdQuery,
} from "../../../../store/api-slice/modelAPISlice.ts";

// Assuming SelectItem is something like:
// interface SelectItem {
//   name: string;
//   value: string;
// }

export const useModelSchema = (
  modelId?: string,
): { schema: ObjectSchema<ModelCreateOrEdit>; isLoading: boolean } => {
  const { data: modelDetail, isLoading } = useGetModelsByIdQuery(modelId!, {
    skip: !modelId,
  });

  return {
    schema: useMemo(() => {
      return yup.object().shape({
        name: yup
          .string()
          .default(modelDetail?.name || "")
          .required("Name is required"),

        roles: yup
          .array()
          .of(
            yup.object({
              name: yup.string().required("Role name is required"),
              value: yup.string().required("Role value is required"),
            }),
          )
          .min(1, "At least one role is required")
          .default(modelDetail?.roles || []),

        model_path: yup
          .object({
            name: yup.string().required("Model path name is required"),
            value: yup.string().required("Model path value is required"),
          })
          .required("Model Path is required")
          .default(modelDetail?.model_path || {}),

        max_tokens: yup
          .number()
          .integer("Max tokens must be an integer")
          .positive("Max tokens must be a positive number")
          .nullable()
          .transform((value, originalValue) =>
            String(originalValue).trim() === "" ? null : value,
          )
          .default(modelDetail?.max_tokens || null),

        temperature: yup
          .number()
          .min(0, "Temperature must be at least 0")
          .max(2, "Temperature must not exceed 2")
          .nullable()
          .transform((value, originalValue) =>
            String(originalValue).trim() === "" ? null : value,
          )
          .default(modelDetail?.temperature || null),

        context_length: yup
          .number()
          .integer("Context length must be an integer")
          .positive("Context length must be a positive number")
          .nullable()
          .transform((value, originalValue) =>
            String(originalValue).trim() === "" ? null : value,
          )
          .default(modelDetail?.context_length || null),

        threshold: yup
          .number()
          .integer("Token limit threshold must be an integer")
          .positive("Token limit threshold must be a positive number")
          .nullable()
          .transform((value, originalValue) =>
            String(originalValue).trim() === "" ? null : value,
          )
          .default(modelDetail?.threshold || null),

        consumption_range_hours: yup
          .number()
          .integer("Consumption range must be an integer")
          .positive("Consumption range must be a positive number")
          .nullable()
          .transform((value, originalValue) =>
            String(originalValue).trim() === "" ? null : value,
          )
          .default(modelDetail?.consumption_range_hours || null),
      });
    }, [modelDetail]) as ObjectSchema<ModelCreateOrEdit>,
    isLoading,
  };
};
