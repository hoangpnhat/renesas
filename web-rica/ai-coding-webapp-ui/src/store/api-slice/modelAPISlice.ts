/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { createApi } from "@reduxjs/toolkit/query/react";
import { END_POINT } from "../../requests/endpoint.ts";

import customAxiosBaseQuery from "./axiosBaseQuery.ts";
import { FilterParams } from "../../typings/common.props.ts";
import { BaseInterface } from "../../typings";
import { GetBaseResponseBody } from "../../typings/request.ts";
import type { SelectItem } from "../../components/form/CommonSelections.tsx";
import capitalize from "lodash/capitalize";

interface DefaultCompletionOptions {
  max_tokens?: number;
  temperature?: number;
  context_length?: number;
}

export interface ModelCreateOrEdit extends DefaultCompletionOptions {
  name: string;
  model_path: SelectItem;
  roles: SelectItem[];
  temperature?: number;
  threshold?: number | null;
  consumption_range_hours?: number | null;
}

export interface ModelCreateOrEditRequest
  extends Omit<ModelCreateOrEdit, "max_tokens" | "model_path" | "roles"> {
  default_completion_options?: DefaultCompletionOptions;
  roles: string[];
  model_path: string;
}

export interface ModelManagement extends ModelCreateOrEdit, BaseInterface {
  created_at: string;
  id: string;
}

export interface ModelManagementResponse extends ModelCreateOrEditRequest {
  created_at: string;
  id: string;
}

export const modelAPISlice = createApi({
  reducerPath: "modelManagementApi",
  baseQuery: customAxiosBaseQuery,
  tagTypes: [
    "ModelManagement",
    "ModelDatabricks",
    "ModelRoles",
    "ModelConfiguration",
  ],
  endpoints: (builder) => ({
    getModelDatabricks: builder.query<SelectItem[], void>({
      query: () => ({
        url: END_POINT.admin.modelManagement.modelDatabricks,
        method: "get",
      }),
      providesTags: [{ type: "ModelDatabricks", id: "LIST" }],
    }),
    getModelRolesList: builder.query<SelectItem[], void>({
      query: () => ({
        url: END_POINT.admin.modelManagement.modelRoles,
        method: "get",
      }),
      providesTags: [{ type: "ModelRoles", id: "LIST" }],
    }),
    getModelsConfiguration: builder.query<string, void>({
      query: () => ({
        url: END_POINT.admin.modelManagement.configuration,
        method: "get",
      }),
      providesTags: [{ type: "ModelConfiguration", id: "LIST" }],
      transformResponse: (response: any) => JSON.stringify(response, null, 2),
    }),
    getModels: builder.query<
      GetBaseResponseBody<ModelManagement>,
      FilterParams
    >({
      query: ({ page = 1, entries = 10, query = "" }) => ({
        url: END_POINT.admin.modelManagement.base,
        method: "get",
        params: {
          page,
          entries: entries || null,
          query: query || null,
        },
      }),
      serializeQueryArgs: ({ queryArgs }) => {
        // Use only the limit as part of the cache key
        // This allows us to keep a single cache entry for all pages
        return `getTopics-${queryArgs.entries}-${queryArgs.query}`;
      },
      merge: (currentCache, newItems, { arg }) => {
        if (arg.page === 0) {
          // Replace cache if it's the first page
          return newItems;
        }

        return {
          ...newItems,
          data: [...(currentCache?.data || []), ...(newItems?.data || [])],
          // Keep track of pagination metadata
          currentPage: arg.page,
          // Preserve the total and totalPages from the new response
          total: newItems.total,
        };
      },
      forceRefetch({ currentArg, previousArg }) {
        if (!previousArg) return true;

        // Refetch if:
        // 1. The limit changes (items per page)
        // 2. We're loading a new page
        // 3. We're refreshing the same page (page 1)
        return (
          currentArg?.entries !== previousArg?.entries ||
          currentArg?.page !== previousArg?.page ||
          (currentArg?.page === 1 && previousArg?.page === 1)
        );
      },
      providesTags: (result) => {
        // If we have no result, provide a LIST tag only
        if (!result?.data)
          return [{ type: "ModelManagement" as const, id: "LIST" }];

        // Otherwise, provide a LIST tag and a tag for each item
        return [
          { type: "ModelManagement" as const, id: "LIST" },
          ...result.data.map((item) => ({
            type: "ModelManagement" as const,
            id: item.id,
          })),
        ];
      },
      transformResponse: (response: GetBaseResponseBody<ModelManagement>) => {
        return {
          ...response,
          // Ensure data is always an array
          data: response.data || [],
          // Add pagination metadata if not present
          total: response.total || response.data?.length || 0,
          currentPage: response.page || 1,
        };
      },
    }),
    getModelsById: builder.query<ModelCreateOrEdit, string>({
      query: (id) => ({
        url: `${END_POINT.admin.modelManagement.base}/${id}`,
        method: "get",
      }),
      providesTags: (result, _, id) =>
        result ? [{ type: "ModelManagement", id }] : [],
      transformResponse: (response: ModelManagementResponse) => {
        return {
          model_path: {
            name: response.model_path || "",
            value: response.model_path || "",
          },
          name: response.name,
          roles: response.roles.map((role) => ({
            name: capitalize(role),
            value: role,
          })),
          max_tokens: response.default_completion_options?.max_tokens,
          temperature: response.default_completion_options?.temperature,
          context_length: response.default_completion_options?.context_length,
          threshold: response.threshold ?? null,
          consumption_range_hours: response.consumption_range_hours ?? null,
        };
      },
    }),
    createModel: builder.mutation<ModelManagement, ModelCreateOrEditRequest>({
      query: (bodyRequest) => ({
        url: END_POINT.admin.modelManagement.base,
        method: "post",
        data: bodyRequest,
      }),
    }),
    deleteModelById: builder.mutation<void, string>({
      query: (id) => ({
        url: `${END_POINT.admin.modelManagement.base}/${id}`,
        method: "delete",
        headers: { skipLoading: false },
      }),
    }),

    updateModel: builder.mutation<
      ModelManagement,
      { id: string; requestBody: ModelCreateOrEditRequest; entries: number }
    >({
      query: ({ id, requestBody }) => ({
        url: `${END_POINT.admin.modelManagement.base}/${id}`,
        method: "patch",
        data: requestBody,
      }),
      invalidatesTags: (_result, _error, { id }) => [
        { type: "ModelManagement", id },
        { type: "ModelManagement", id: "LIST" },
      ],
    }),
  }),
});
export const {
  useGetModelsQuery,
  useCreateModelMutation,
  useDeleteModelByIdMutation,
  useGetModelsByIdQuery,
  useUpdateModelMutation,
  useGetModelDatabricksQuery,
  useGetModelRolesListQuery,
  useLazyGetModelsConfigurationQuery,
} = modelAPISlice;
