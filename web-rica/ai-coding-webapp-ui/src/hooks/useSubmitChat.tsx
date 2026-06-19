/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import { useNavigate, useParams } from "react-router-dom";
import { Message } from "../typings/request";
import { useDispatch, useSelector } from "react-redux";
import { RootState } from "../store";
import { createDialogChat } from "../pages/requests/chat-request";
import { addLatestDialog } from "../store/reducers/dialog.slice";
import { useTranslation } from "react-i18next";
import { Language } from "../typings/common.props.ts";
import { acquireToken } from "../msal/auth.tsx";
import { updateCommon } from "../store/reducers/common.slice.ts";

export const useSubmitChat = (handleSubmit: (message: Message) => void) => {
  // TODO: Handle socket io events for chatting
  // [x]: Context API
  const navigate = useNavigate();
  const dispatch = useDispatch();
  const input = useSelector((state: RootState) => state.input);
  const { dialog_id } = useParams();
  const { i18n } = useTranslation();
  const handleSubmitChat = async (message: Partial<Message>) => {
    if (!input.isLoading) {
      dispatch(updateCommon({ isThinkingChatBot: true }));
      let dialog_id_req = dialog_id;

      if (dialog_id_req === "undefined" || !dialog_id_req) {
        const response = await createDialogChat({ title: "New chat" });
        dialog_id_req = response.data.id;
        dispatch(
          addLatestDialog({
            title: response.data.title,
            id: response.data.id,
            datetime: response.data.last_modified,
          }),
        );
        navigate(`/chat/${dialog_id_req}`);
      }

      const bodyRequest: Partial<Message> = {
        ...message,
        role: "user",
        dialog_id: dialog_id_req as string,
        language: i18n.language as Language,
        token: `${await acquireToken()}`,
      };

      handleSubmit?.(bodyRequest as Message);
    }
  };
  return { handleSubmitChat };
};
