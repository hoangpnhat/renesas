import { createApi } from "@reduxjs/toolkit/query/react";
import { END_POINT } from "../../requests/endpoint.ts";
import { BaseListResponse } from "../../typings/request.ts";
import customAxiosBaseQuery from "../api-slice/axiosBaseQuery.ts";
import { SharedResource } from "./knowledgeBaseAPISlice.ts";

const CustomizeEndpoint = END_POINT.customize.prompt;

export type CustomizeType = "rule" | "prompt";

// Truncated shape returned by the list endpoint
export interface CustomizeListItem {
  id: string;
  name: string;
  description: string;
  type: CustomizeType;
  created_at: string;
  last_modified: string;
}

export interface CustomizeFileInfo {
  prompt_file_name: string;
  prompt_file_path: string;
  prompt_file_size: number;
  prompt_file_extension: string;
}

// Full shape returned by the detail endpoint
export interface CustomizeItem extends CustomizeListItem {
  status: string;
  visibility: string;
  owner_id: string;
  owner_name: string;
  prompt_file_info: CustomizeFileInfo;
  current_user_role?: string;
  can_view?: boolean;
  can_share?: boolean;
  can_edit?: boolean;
  can_delete?: boolean;
  can_add_files?: boolean;
  can_remove_files?: boolean;
  can_archive?: boolean;
  is_owner?: boolean;
  is_deleted?: boolean;
  is_archived?: boolean;
}

export interface ShareCustomizeRequest {
  target_id: string;
  target_type: "user" | "group";
  role: "viewer" | "developer" | "owner";
}

export const customizeApiSlice = createApi({
  reducerPath: "apiCustomize",
  baseQuery: customAxiosBaseQuery,
  tagTypes: ["Customize"],
  endpoints: (builder) => ({
    getCustomizes: builder.query<
      BaseListResponse<CustomizeListItem>,
      {
        page: number;
        entries: number;
        query?: string;
        prompt_type?: CustomizeType;
      }
    >({
      query: ({ page = 0, entries = 10, query = "", prompt_type }) => ({
        url: CustomizeEndpoint,
        method: "get",
        params: {
          page: page + 1,
          limit: entries,
          query,
          ...(prompt_type && { prompt_type }),
          sort_field: "last_modified",
        },
      }),
      providesTags: (result) =>
        result
          ? [
              ...result.data.map(({ id }: { id: string }) => ({
                type: "Customize" as const,
                id,
              })),
              { type: "Customize" as const, id: "LIST" },
            ]
          : [{ type: "Customize" as const, id: "LIST" }],
    }),
    getCustomizeDetail: builder.query<CustomizeItem, string>({
      query: (id) => ({
        url: `${CustomizeEndpoint}/${id}`,
        method: "get",
      }),
      providesTags: (result, _, id) =>
        result ? [{ type: "Customize", id }] : [],
    }),
    createCustomize: builder.mutation<CustomizeItem, FormData>({
      query: (formData) => ({
        url: CustomizeEndpoint,
        method: "post",
        data: formData,
        headers: { "Content-Type": "multipart/form-data" },
      }),
      invalidatesTags: [{ type: "Customize", id: "LIST" }],
    }),
    updateCustomize: builder.mutation<
      CustomizeItem,
      { id: string; formData: FormData }
    >({
      query: ({ id, formData }) => ({
        url: `${CustomizeEndpoint}/${id}`,
        method: "patch",
        data: formData,
        headers: { "Content-Type": "multipart/form-data" },
      }),
      invalidatesTags: (_, __, { id }) => [
        { type: "Customize", id },
        { type: "Customize", id: "LIST" },
      ],
    }),
    deleteCustomize: builder.mutation<{ is_deleted_ok: boolean }, string>({
      query: (id) => ({
        url: `${CustomizeEndpoint}/${id}`,
        method: "delete",
      }),
      invalidatesTags: [{ type: "Customize", id: "LIST" }],
    }),
    getCustomizeFileContent: builder.query<Blob, string>({
      query: (id) => ({
        url: `${CustomizeEndpoint}/download/${id}`,
        method: "get",
        responseType: "blob",
      }),
    }),
    getCustomizeShares: builder.query<SharedResource[], string>({
      query: (id) => ({
        url: `${CustomizeEndpoint}/${id}/share`,
        method: "get",
      }),
      providesTags: (result, _, id) =>
        result ? [{ type: "Customize", id: `${id}-shares` }] : [],
    }),
    shareCustomize: builder.mutation<
      void,
      { id: string; requestBody: ShareCustomizeRequest[] }
    >({
      query: ({ id, requestBody }) => ({
        url: `${CustomizeEndpoint}/${id}/share`,
        method: "patch",
        data: requestBody,
      }),
      invalidatesTags: (_, __, { id }) => [
        { type: "Customize", id },
        { type: "Customize", id: `${id}-shares` },
      ],
    }),
  }),
});

export const {
  useGetCustomizesQuery,
  useGetCustomizeDetailQuery,
  useGetCustomizeFileContentQuery,
  useCreateCustomizeMutation,
  useUpdateCustomizeMutation,
  useDeleteCustomizeMutation,
  useGetCustomizeSharesQuery,
  useShareCustomizeMutation,
} = customizeApiSlice;
