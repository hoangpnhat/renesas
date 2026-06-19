import { createApi } from "@reduxjs/toolkit/query/react";
import { END_POINT } from "../../requests/endpoint";
import {
  Group,
  GroupsListResponse,
  CreateGroupRequest,
} from "../../typings/request";
import { FilterParams } from "../../typings/common.props";
import customAxiosBaseQuery from "../api-slice/axiosBaseQuery";
import { DynamicFilterParams } from "../../typings/filter.props";

export const groupApiSlice = createApi({
  reducerPath: "apiGroups",
  baseQuery: customAxiosBaseQuery,
  tagTypes: ["Group"],
  endpoints: (builder) => ({
    getGroups: builder.query<GroupsListResponse, Partial<FilterParams>>({
      query: ({ page = 1, entries = 20 }) => ({
        url: `${END_POINT.group}/active`,
        method: "get",
        params: {
          page: page + 1,
          limit: entries,
          sort_field: "last_modified",
        },
      }),
      providesTags: (result) =>
        result
          ? [
              ...result.data.map(({ id }) => ({
                type: "Group" as const,
                id,
              })),
              { type: "Group" as const, id: "LIST" },
            ]
          : [{ type: "Group" as const, id: "LIST" }],
    }),
    searchGroup: builder.query<GroupsListResponse, DynamicFilterParams>({
      query: (data) => ({
        url: `${END_POINT.group}/search`,
        method: "post",
        data: data,
        headers: {
          skipNotification: data?.search_text,
        },
      }),
      providesTags: (result) =>
        result
          ? [
              ...result.data.map(({ id }) => ({
                type: "Group" as const,
                id,
              })),
              { type: "Group" as const, id: "LIST" },
            ]
          : [{ type: "Group" as const, id: "LIST" }],
    }),
    getGroup: builder.query<Group, string>({
      query: (id) => ({
        url: `${END_POINT.group}/${id}`,
        method: "get",
      }),
      providesTags: (result, _, id) => (result ? [{ type: "Group", id }] : []),
    }),
    createGroup: builder.mutation<Group, CreateGroupRequest>({
      query: (data) => ({
        url: END_POINT.group,
        method: "post",
        data,
      }),
      invalidatesTags: [{ type: "Group", id: "LIST" }],
    }),
    updateGroup: builder.mutation<
      Group,
      { id: string; data: Partial<CreateGroupRequest> }
    >({
      query: ({ id, data }) => ({
        url: `${END_POINT.group}/${id}/general`,
        method: "patch",
        data,
      }),
      invalidatesTags: (__, _, { id }) => [
        { type: "Group", id },
        { type: "Group", id: "LIST" },
      ],
    }),
    deleteGroup: builder.mutation<void, string>({
      query: (id) => ({
        url: `${END_POINT.group}/${id}`,
        method: "delete",
      }),
      invalidatesTags: (__, _, id) => [
        { type: "Group", id },
        { type: "Group", id: "LIST" },
      ],
    }),
    joinGroup: builder.mutation<void, string>({
      query: (groupId) => ({
        url: `${END_POINT.group}/${groupId}/members/join`,
        method: "post",
      }),
      invalidatesTags: (__, _, groupId) => [
        { type: "Group", id: groupId },
        { type: "Group", id: "LIST" },
      ],
    }),
    declineGroupInvitation: builder.mutation<void, string>({
      query: (groupId) => ({
        url: `${END_POINT.group}/${groupId}/members/decline`,
        method: "post",
      }),
      invalidatesTags: [{ type: "Group", id: "LIST" }],
    }),
    updateMembers: builder.mutation<
      void,
      { groupId: string; members: { user_id: string; role: string }[] }
    >({
      query: ({ groupId, members }) => ({
        url: `${END_POINT.group}/${groupId}/members`,
        method: "post",
        data: { members },
      }),
      invalidatesTags: (__, _, { groupId }) => [{ type: "Group", id: groupId }],
    }),
  }),
});

export const {
  useGetGroupsQuery,
  useSearchGroupQuery,
  useGetGroupQuery,
  useCreateGroupMutation,
  useUpdateGroupMutation,
  useDeleteGroupMutation,
  useJoinGroupMutation,
  useDeclineGroupInvitationMutation,
  useUpdateMembersMutation,
} = groupApiSlice;
