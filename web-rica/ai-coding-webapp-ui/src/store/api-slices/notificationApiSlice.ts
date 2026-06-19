import { createApi } from "@reduxjs/toolkit/query/react";
import { END_POINT } from "../../requests/endpoint";
import { NotificationsListResponse } from "../../typings/request";
import { FilterParams } from "../../typings/common.props";
import customAxiosBaseQuery from "../api-slice/axiosBaseQuery";

export const notificationApiSlice = createApi({
  reducerPath: "apiNotifications",
  baseQuery: customAxiosBaseQuery,
  tagTypes: ["Notification"],
  endpoints: (builder) => ({
    getNotifications: builder.query<
      NotificationsListResponse,
      Partial<FilterParams>
    >({
      query: ({ page = 1, entries = 20 }) => ({
        url: END_POINT.notifications,
        method: "get",
        params: {
          page: page + 1,
          limit: entries,
        },
        headers: { skipLoading: true },
      }),
      providesTags: (result) =>
        result
          ? [
              ...result.data.map(({ id }) => ({
                type: "Notification" as const,
                id,
              })),
              { type: "Notification" as const, id: "LIST" },
            ]
          : [{ type: "Notification" as const, id: "LIST" }],
    }),
    markNotificationAsRead: builder.mutation<void, string>({
      query: (notificationId) => ({
        url: `${END_POINT.notifications}/${notificationId}/read`,
        method: "post",
        headers: { skipLoading: true },
      }),
      async onQueryStarted(notificationId, { dispatch, queryFulfilled }) {
        const patchResult = dispatch(
          notificationApiSlice.util.updateQueryData(
            "getNotifications",
            { page: 0, entries: 20 },
            (draft) => {
              const notification = draft.data.find(
                (n) => n.id === notificationId,
              );
              if (notification) {
                notification.status = "read";
              }
            },
          ),
        );
        try {
          await queryFulfilled;
        } catch {
          patchResult.undo();
        }
      },
      invalidatesTags: (__, _, id) => [{ type: "Notification", id }],
    }),
  }),
});

export const { useGetNotificationsQuery, useMarkNotificationAsReadMutation } =
  notificationApiSlice;
