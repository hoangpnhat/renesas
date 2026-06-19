/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import { AxiosResponse } from "axios";
import { useMemo } from "react";
import { useDispatch } from "react-redux";
import { useParams } from "react-router-dom";

import ContentCopyIcon from "@mui/icons-material/ContentCopy";
import EditIcon from "@mui/icons-material/Edit";
import ThumbDownIcon from "@mui/icons-material/ThumbDown";
import ThumbDownAltOutlinedIcon from "@mui/icons-material/ThumbDownAltOutlined";
import ThumbUpIcon from "@mui/icons-material/ThumbUp";
import ThumbUpOutlinedIcon from "@mui/icons-material/ThumbUpOutlined";

import { ROLE } from "../../../constants/common";
import { updateContent } from "../../../store/reducers/chat.slice";
import { updateCommon } from "../../../store/reducers/common.slice";
import { updateDialogWithNewItem } from "../../../store/reducers/dialog.slice";
import { ChatToolBarItem, MessageType } from "../../../typings/component.props";
import { MessageContentUpdate, MessageContentUpdateResponseBody } from "../../../typings/request";
import { copyToClip } from "../../../utils/utilities";
import { updateMessageContent } from "../../requests/chat-request";

export const useChatToolBarItem = () => {
  const dispatch = useDispatch();
  const { dialog_id } = useParams();
  const toolbarArr: { [K: string]: ChatToolBarItem[] } = useMemo(
    () => ({
      [ROLE.USER]: [
        {
          title: "Delete",
          name: "delete",
          icon: <EditIcon fontSize="small" />,
          onClickHandle: (_event, item) => {
            console.log("🚀 ~ item:", item);
          },
        },
      ],
      [ROLE.ASSISTANT]: [
        // {
        //   title: "Bookmark",
        //   name: "bookmark",
        //   icon: <Bookmark />,
        //   outlineItem: <BookmarkBorderOutlinedIcon />,
        //   onClickHandle: async (
        //     _event: React.MouseEvent,
        //     item: MessageType
        //   ) => {
        //     if (!!dialog_id && dialog_id !== "undefined") {
        //       const resp: AxiosResponse<SavedChatContentResponseBody> =
        //         await updateDialog(dialog_id as string, item.id, {
        //           is_favorite: !item.is_favorite,
        //         });
        //       dispatch(
        //         updateContent({
        //           id: (resp.data.results as SavedChatContent).content_id,
        //           is_favorite: !(resp.data.results as SavedChatContent)
        //             .is_disabled,
        //         })
        //       );
        //     }
        //   },
        // },
        // {
        //   title: "Suggest Response",
        //   icon: <AssistantIcon />,
        //   name: "suggest_response",
        //   outlineItem: <AssistantOutlinedIcon />,
        //   onClickHandle: async (
        //     _event: React.MouseEvent,
        //     item: MessageType
        //   ) => {
        //     dispatch(
        //       updateDialogConfig({
        //         hasAction: false,
        //         maxWidth: "lg",
        //         fullWidth: true,
        //         isOpen: true,
        //         title: "Suggest Response",
        //         children: (
        //           <EditTitle
        //             valueDefault={item.content}
        //             onHandleSubmit={async (value: string) => {
        //               const request_body: Partial<MessageContentUpdate> = {
        //                 content: value,
        //                 role: item.role,
        //                 dialog_id: dialog_id,
        //                 categories: item.categories,
        //                 vector_search: item.vector_search,
        //               };
        //               const responseBody: AxiosResponse<MessageContentUpdateResponseBody> =
        //                 await updateMessageContent(
        //                   dialog_id as string,
        //                   item.id,
        //                   request_body as MessageContentUpdate
        //                 );
        //               dispatch(
        //                 updateContent({
        //                   ...responseBody.data.message_content,
        //                   status: "",
        //                 })
        //               );

        //               dispatch(
        //                 updateDialogWithNewItem({
        //                   title: responseBody.data.dialog.title,
        //                   datetime: responseBody.data.dialog.last_modified,
        //                   id: responseBody.data.dialog.id,
        //                 })
        //               );
        //             }}
        //           />
        //         ),
        //       })
        //     );
        //   },
        // },
        {
          title: "title.goodResponse",
          outlineItem: <ThumbUpOutlinedIcon fontSize="small" />,

          name: "best_response",
          icon: <ThumbUpIcon fontSize="small" />,
          onClickHandle: async (
            _event: React.MouseEvent,
            item: MessageType,
          ) => {
            const request_body: Partial<MessageContentUpdate> = {
              role: item.role,
              dialog_id: dialog_id,
              is_good_response: item.is_good_response === 1 ? -1 : 1,
            };
            const responseBody: AxiosResponse<MessageContentUpdateResponseBody> =
              await updateMessageContent(
                dialog_id as string,
                item.id,
                request_body as MessageContentUpdate,
              );
            dispatch(
              updateContent({
                ...responseBody.data.message_content,
                status: "",
              }),
            );

            dispatch(
              updateDialogWithNewItem({
                title: responseBody.data.dialog.title,
                datetime: responseBody.data.dialog.last_modified,
                id: responseBody.data.dialog.id,
              }),
            );
          },
        },
        {
          title: "title.badResponse",
          outlineItem: <ThumbDownAltOutlinedIcon fontSize="small" />,

          name: "bad_response",
          icon: <ThumbDownIcon fontSize="small" />,
          onClickHandle: async (
            _event: React.MouseEvent,
            item: MessageType,
          ) => {
            const request_body: Partial<MessageContentUpdate> = {
              role: item.role,
              dialog_id: dialog_id,
              is_good_response: item.is_good_response === 0 ? -1 : 0,
            };
            const responseBody: AxiosResponse<MessageContentUpdateResponseBody> =
              await updateMessageContent(
                dialog_id as string,
                item.id,
                request_body as MessageContentUpdate,
              );
            dispatch(
              updateContent({
                ...responseBody.data.message_content,
                status: "",
              }),
            );

            dispatch(
              updateDialogWithNewItem({
                title: responseBody.data.dialog.title,
                datetime: responseBody.data.dialog.last_modified,
                id: responseBody.data.dialog.id,
              }),
            );
          },
        },
        {
          title: "title.copyThisResponse",
          outlineItem: <ContentCopyIcon fontSize="small" />,

          name: "copy_response",
          icon: <ContentCopyIcon fontSize="small" />,
          onClickHandle: async (
            _event: React.MouseEvent,
            item: MessageType,
          ) => {
            copyToClip(item.content);
            dispatch(
              updateCommon({
                snackBar: { isOpen: true, message: "Copied", type: "info" },
              }),
            );
          },
        },
      ],
    }),
    [dialog_id],
  );
  return { toolbarArr };
};
