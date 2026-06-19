/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */
import { UnknownAction } from "@reduxjs/toolkit";
import { createApi } from "@reduxjs/toolkit/query/react";
import { AxiosProgressEvent } from "axios";
import { Dispatch } from "react";
import { ENTRIES_STANDARD_RECORDS } from "../../constants/common.ts";
import { END_POINT } from "../../requests/endpoint.ts";
import { BaseInterface } from "../../typings";
import { FilterParams } from "../../typings/common.props.ts";
import { BaseListResponse } from "../../typings/request.ts";
import { updateCommon } from "../reducers/common.slice.ts";
import { updateProgress } from "../reducers/progress.slice.ts";
import customAxiosBaseQuery from "../api-slice/axiosBaseQuery.ts";

export interface FileManagement extends BaseInterface {
  file_name: string;
  owner: string;
  uploaded_at: string;
  last_modified?: string;
  modified?: string;
  is_owner: boolean;
  total_pages?: number;
  size: number;
  file_path: string;
  status: FileStatus;
}

export interface FileDetail extends FileManagement {
  extension: string;
}

export interface FileCreateOrEdit {
  file: File;
}

export enum FileStatus {
  processing = "processing",
  completed = "completed",
  failed = "failed",
}

export const fileApiSlice = createApi({
  reducerPath: "apiFiles",
  baseQuery: customAxiosBaseQuery,
  tagTypes: ["File"],
  endpoints: (builder) => ({
    getFiles: builder.query<
      BaseListResponse<FileManagement>,
      Partial<FilterParams>
    >({
      query: ({ page = 1, entries = 10, query = "" }) => ({
        url: END_POINT.file,
        method: "get",
        params: {
          page: page + 1 || null,
          limit: entries || null,
          query: query || "",
          sort_field: "modified",
          sort_order: "desc",
        },
      }),
      providesTags: (result) =>
        result
          ? [
              ...result.data.map(({ id }) => ({
                type: "File" as const,
                id,
              })),
              { type: "File" as const, id: "LIST" },
            ]
          : [{ type: "File" as const, id: "LIST" }],
    }),
    getFileDetail: builder.query<FileDetail, string>({
      query: (id) => ({
        url: `${END_POINT.file}/${id}`,
        method: "get",
        headers: { skipLoading: true },
      }),
      providesTags: (result, _, id) => (result ? [{ type: "File", id }] : []),
    }),
    uploadFile: builder.mutation<
      FileManagement,
      {
        bodyRequest: FormData;
        dispatch: Dispatch<UnknownAction>;
      }
    >({
      query: ({ bodyRequest, dispatch }) => ({
        url: END_POINT.file,
        method: "post",
        data: bodyRequest,
        headers: {
          "Content-Type": "multipart/form-data",
          skipNotification: true,
          skipLoading: true,
        },
        onUploadProgress: function (progressEvent: AxiosProgressEvent) {
          const percent = Math.round(
            (progressEvent.loaded * 100) / (progressEvent?.total ?? 1),
          );

          const fileRequest = bodyRequest.get("file");
          if (fileRequest) {
            dispatch(
              updateProgress({
                documentId: (fileRequest as File).name,
                loadingPercent: percent,
                status: "pending",
                isLoading: true,
              }),
            );
          }
        },
      }),
      invalidatesTags: [{ type: "File", id: "LIST" }],

      async onQueryStarted(_args, { dispatch, queryFulfilled }) {
        try {
          const { data } = await queryFulfilled;

          dispatch(
            updateProgress({
              documentId: data.file_name,
              status: "fulfilled",
              isLoading: false,
            }),
          );

          dispatch(
            updateCommon({
              snackBar: {
                isOpen: true,
                message: "File uploaded successfully",
                type: "success",
              },
            }),
          );

          dispatch(
            fileApiSlice.util.updateQueryData(
              "getFiles",
              { page: 0, entries: ENTRIES_STANDARD_RECORDS, query: "" },
              (draft) => {
                return {
                  ...draft,
                  pagination: {
                    ...draft.pagination,
                    total: draft.pagination.total + 1,
                  },
                  data: [data, ...draft.data],
                };
              },
            ),
          );
        } catch (err) {
          if (err && typeof err === "object" && "error" in err) {
            if (
              err["error"] &&
              typeof err["error"] === "object" &&
              "payload" in err["error"] &&
              typeof err.error.payload === "string"
            )
              dispatch(
                updateProgress({
                  documentId: err.error.payload,
                  status: "error",
                  isLoading: false,
                }),
              );
          }
        }
      },
    }),

    getFilePreviewMeta: builder.query<
      { file_id: string; total_pages: number },
      string
    >({
      query: (fileId) => ({
        url: `${END_POINT.file}/${fileId}/preview`,
        method: "get",
        headers: { skipLoading: "true" },
      }),
    }),

    deleteFileWithID: builder.mutation<
      null,
      { id: string } & Partial<FilterParams>
    >({
      query: ({ id }) => ({
        url: `${END_POINT.file}/${id}`,
        method: "delete",
      }),
      invalidatesTags: (_result, _error, { id }) => [
        { type: "File", id: "LIST" },
        { type: "File", id },
      ],
    }),
  }),
});
export const {
  useGetFilesQuery,
  useGetFileDetailQuery,
  useUploadFileMutation,
  useDeleteFileWithIDMutation,
  useLazyGetFilePreviewMetaQuery,
} = fileApiSlice;
