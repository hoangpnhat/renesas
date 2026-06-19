/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { createApi } from "@reduxjs/toolkit/query/react";
import orderBy from "lodash/orderBy";
import { END_POINT } from "../../requests/endpoint.ts";
import { BaseListResponse } from "../../typings/request.ts";
import customAxiosBaseQuery from "../api-slice/axiosBaseQuery.ts";
import { DynamicFilterParams } from "../../typings/filter.props.ts";
export interface FileJunction {
  file_id: string;
  added_at: string;
  file_name: string;
}

export interface MemberShip {
  user_id: string;
  full_name: string;
  role: string;
  added_at: Date;
}
export interface KnowledgeBaseDetail {
  files: FileJunction[];
  name: string;
  description: string;
  members: MemberShip[];
  id: string;
}
export interface KnowledgeBaseAll extends KnowledgeBaseDetail {
  owner_id: string;
  name: string;
  created_at: Date;
  last_modified: Date;
  is_default: boolean;
  author: string;
  is_owner: boolean;
  is_deleted?: boolean;
  owner: string;
  can_share?: boolean;
  can_edit?: boolean;
  can_delete?: boolean;
  current_user_role: string;
}

export interface KnowledgeBaseCreateOrEdit<T> {
  name: string;
  description?: string;
  files?: T;
}

export interface QueryWithPagination {
  page: number;
  entries: number;
  query: string | null;
}

export interface ShareKnowledgeRequest {
  target_id: string;
  target_type: "user" | "group";
  role: "viewer" | "developer" | "owner";
}

export interface SharedResource {
  resource_id: string;
  resource_type: "user" | "group";
  resource_name: string;
  last_modified: string;
  role?: "viewer" | "developer" | "owner";
}

export const knowledgeBaseAPISlice = createApi({
  reducerPath: "apiKnowledgeBase",
  baseQuery: customAxiosBaseQuery,
  tagTypes: ["Knowledge"],
  endpoints: (builder) => ({
    getDefaultKnowledgeBase: builder.query<KnowledgeBaseAll, void>({
      query: () => ({
        url: `${END_POINT.knowledge}/default`,
        method: "get",
      }),

      providesTags: (result) =>
        result ? [{ type: "Knowledge", id: "default" }] : [],
    }),
    getKnowledgeBases: builder.query<
      BaseListResponse<KnowledgeBaseAll>,
      { page: number; entries: number; query: string | null }
    >({
      query: ({ page = 1, entries = 10, query = "" }) => ({
        url: END_POINT.knowledge,
        method: "get",
        params: {
          page: page + 1,
          limit: entries,
          query,
          sort_field: "last_modified",
        },
      }),
      transformResponse: (
        resp: BaseListResponse<KnowledgeBaseAll>,
      ): BaseListResponse<KnowledgeBaseAll> => {
        return {
          ...resp,
          data: orderBy(resp.data, ["is_default"], ["desc"]),
        };
      },
      providesTags: (result) =>
        result
          ? [
              ...result.data.map(({ id }) => ({
                type: "Knowledge" as const,
                id,
              })),
              { type: "Knowledge" as const, id: "LIST" },
            ]
          : [{ type: "Knowledge" as const, id: "LIST" }],
    }),
    searchKnowledgeBase: builder.query<
      BaseListResponse<KnowledgeBaseAll>,
      DynamicFilterParams
    >({
      query: (data) => ({
        url: `${END_POINT.knowledge}/search`,
        method: "post",
        data: data,
        headers: {
          skipNotification: data?.search_text,
        },
      }),
      transformResponse: (
        resp: BaseListResponse<KnowledgeBaseAll>,
      ): BaseListResponse<KnowledgeBaseAll> => {
        return {
          ...resp,
          data: orderBy(resp.data, ["is_default"], ["desc"]),
        };
      },
      providesTags: (result) =>
        result
          ? [
              ...result.data.map(({ id }) => ({
                type: "Knowledge" as const,
                id,
              })),
              { type: "Knowledge" as const, id: "LIST" },
            ]
          : [{ type: "Knowledge" as const, id: "LIST" }],
    }),
    getKnowledgeBaseDetail: builder.query<KnowledgeBaseDetail, string>({
      query: (id) => ({
        url: `${END_POINT.knowledge}/${id}`,
        method: "get",
        headers: { skipLoading: true },
      }),
      providesTags: (result, _, id) =>
        result ? [{ type: "Knowledge", id }] : [],
    }),
    createKnowledgeBase: builder.mutation<
      KnowledgeBaseAll,
      KnowledgeBaseCreateOrEdit<FileJunction[]>
    >({
      query: (bodyRequest) => ({
        url: END_POINT.knowledge,
        method: "post",
        data: bodyRequest,
      }),
      invalidatesTags: [{ type: "Knowledge", id: "LIST" }],
    }),
    updateKnowledgeBase: builder.mutation<
      KnowledgeBaseAll,
      {
        id: string;
        requestBody: Partial<KnowledgeBaseCreateOrEdit<FileJunction[]>>;
      }
    >({
      query: ({ id, requestBody }) => ({
        url: `${END_POINT.knowledge}/${id}/general`,
        method: "put",
        data: requestBody,
      }),
      invalidatesTags: (_, __, { id }) => [{ type: "Knowledge", id }],
    }),
    deleteKnowledgeBaseWithId: builder.mutation<
      { is_deleted_ok: boolean },
      string
    >({
      query: (id) => ({
        url: `${END_POINT.knowledge}/${id}`,
        method: "delete",
      }),
      invalidatesTags: [{ type: "Knowledge", id: "LIST" }],
    }),
    getKnowledgeShares: builder.query<SharedResource[], string>({
      query: (id) => ({
        url: `${END_POINT.knowledge}/${id}/share`,
        method: "get",
      }),
      providesTags: (result, _, id) =>
        result ? [{ type: "Knowledge", id: `${id}-shares` }] : [],
    }),
    shareKnowledge: builder.mutation<
      void,
      { id: string; requestBody: ShareKnowledgeRequest[] }
    >({
      query: ({ id, requestBody }) => ({
        url: `${END_POINT.knowledge}/${id}/share`,
        method: "post",
        data: requestBody,
      }),
      invalidatesTags: (_, __, { id }) => [
        { type: "Knowledge", id },
        { type: "Knowledge", id: `${id}-shares` },
      ],
    }),
  }),
});
export const {
  useGetKnowledgeBasesQuery,
  useSearchKnowledgeBaseQuery,
  useGetKnowledgeBaseDetailQuery,
  useCreateKnowledgeBaseMutation,
  useDeleteKnowledgeBaseWithIdMutation,
  useUpdateKnowledgeBaseMutation,
  useGetKnowledgeSharesQuery,
  useShareKnowledgeMutation,
  useLazyGetKnowledgeBasesQuery,
} = knowledgeBaseAPISlice;
