/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { createApi } from "@reduxjs/toolkit/query/react";
import { User, UserDefaultSetting } from "../../typings/component.props.ts";
import { END_POINT } from "../../requests/endpoint.ts";
import { BaseListResponse } from "../../typings/request.ts";
import customAxiosBaseQuery from "../api-slice/axiosBaseQuery.ts";

export interface UserUpdateMongo {
  full_name: string;
  is_active: boolean;
  defaults: UserDefaultSetting;
}

export interface UserMongo {
  email: string;
  full_name: string;
  is_active: boolean;
  created_at: string;
  last_modified: string;
  defaults: UserDefaultSetting;
}

export const userAPISlice = createApi({
  reducerPath: "apiUser",
  baseQuery: customAxiosBaseQuery,
  tagTypes: ["UserMongo"],
  endpoints: (builder) => ({
    getMe: builder.query<User & { defaults: UserDefaultSetting }, void>({
      query: () => ({
        url: END_POINT.userMe,
        method: "get",
        headers: { skipLoading: true },
      }),
      transformResponse: (
        response: User & { defaults: UserDefaultSetting },
      ) => {
        return response;
      },
    }),
    getUsers: builder.query<
      BaseListResponse<User>,
      { query: string; limit: number }
    >({
      query: ({ query = "", limit = 5 }) => ({
        url: END_POINT.userSearch,
        method: "get",
        headers: {
          skipLoading: true,
        },
        params: { query, limit },
      }),
      keepUnusedDataFor: 5,
      providesTags: (result: any) =>
        result
          ? [
              ...result.data.map(({ user_id }: { user_id: any }) => ({
                type: "UserMongo" as const,
                user_id,
              })),
              { type: "UserMongo" as const, id: "LIST" },
            ]
          : [{ type: "UserMongo" as const, id: "LIST" }],
    }),
    updateUser: builder.mutation<UserMongo, Partial<UserUpdateMongo>>({
      query: (bodyRequest: Partial<UserUpdateMongo>) => ({
        url: END_POINT.user,
        method: "patch",
        data: bodyRequest,
      }),
      async onQueryStarted(
        bodyRequest: any,
        { dispatch, queryFulfilled }: { dispatch: any; queryFulfilled: any },
      ) {
        const patchResult = dispatch(
          userAPISlice.util.updateQueryData("getMe", undefined, (draft) => {
            return {
              ...draft,
              ...bodyRequest,
            };
          }),
        );

        try {
          await queryFulfilled;
        } catch (e) {
          patchResult.undo();
        }
      },
    }),
  }),
});
export const { useGetUsersQuery, useGetMeQuery, useUpdateUserMutation } =
  userAPISlice;
