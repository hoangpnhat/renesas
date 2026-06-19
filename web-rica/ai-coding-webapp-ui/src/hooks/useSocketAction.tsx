/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import { useDispatch } from "react-redux";
import {
  ContentDialogResults,
  DialogResult,
  Message,
} from "../typings/request";
import { getSocket } from "./useSocketIo";
import { updateCommon } from "../store/reducers/common.slice";
import { useEffect, useState } from "react";
import {
  newContent,
  removeErrorMessage,
  updateContentWithStatus,
} from "../store/reducers/chat.slice";
import { ROLE } from "../constants/common";
import { MessageType } from "../typings/component.props";
import { updateDialogWithNewItem } from "../store/reducers/dialog.slice";
import { updateText } from "../store/reducers/input.slice";
import { Socket } from "socket.io-client";
import { toast } from "react-toastify";
import { fileApiSlice, FileStatus } from "../store/api-slices/fileApiSlice";
import { store } from "../store";

interface FileStatusMessage {
  file_id: string;
  file_name: string;
  status: FileStatus;
}

export const useSocketAction = () => {
  const dispatch = useDispatch();
  const [socket, setSocket] = useState<Socket | null>(null);

  const handleSubmit = (message: Message) => {
    if (socket) {
      socket.emit("message", message);
      const newData: Partial<MessageType> = {
        ...message,
        status: "queuing",
        role: ROLE.USER,
      };
      dispatch(
        newContent([
          {
            status: "pending",
            role: ROLE.ASSISTANT,
          } as MessageType,
          newData as MessageType,
        ]),
      );
    }
  };

  useEffect(() => {
    let mounted = true;

    getSocket().then((socketInstance) => {
      if (!mounted) return;

      setSocket(socketInstance);
      socketInstance.connect();

      function onConnect() {
        console.log("Socket connected successfully");
      }

      function onMessageError(message: { data: ContentDialogResults }) {
        dispatch(removeErrorMessage({ errorId: message.data.id }));
        dispatch(updateText(message.data.content));
        dispatch(
          updateCommon({
            isThinkingChatBot: false,
            snackBar: {
              isOpen: true,
              type: "error",
              message: "There are something went wrong, please try again",
            },
          }),
        );
      }

      function onMessage(message: { data: ContentDialogResults }) {
        const newData: Partial<MessageType> = {
          ...(message.data as MessageType),
          status: "completed",
        };
        if (message.data.role === ROLE.ASSISTANT) {
          dispatch(
            updateContentWithStatus({ ...newData, statusToUpdate: "pending" }),
          );
        } else if (message.data.role === ROLE.USER) {
          dispatch(
            updateContentWithStatus({ ...newData, statusToUpdate: "queuing" }),
          );
        }
      }

      function onUpdateTitle(message: { data: DialogResult }) {
        dispatch(updateCommon({ isSelectedSidebar: message.data.id }));
        dispatch(
          updateDialogWithNewItem({
            title: message.data.title,
            id: message.data.id,
            datetime: message.data.last_modified,
          }),
        );
      }

      function onFileStatus(message: { data: FileStatusMessage }) {
        console.log(message.data);
        const { file_id, file_name, status } = message.data;

        // Show toast notification
        const toastType =
          status === FileStatus.completed
            ? "success"
            : status === FileStatus.failed
              ? "error"
              : "info";
        toast[toastType](`${file_name} processed ${status}`);

        // Update file status in all cached RTK Query entries
        const state = store.getState();
        const apiState = state[fileApiSlice.reducerPath];

        Object.keys(apiState.queries).forEach((key) => {
          if (key.startsWith("getFiles")) {
            const queryArgs = apiState.queries[key]?.originalArgs;
            if (queryArgs) {
              store.dispatch(
                fileApiSlice.util.updateQueryData(
                  "getFiles",
                  queryArgs,
                  (draft) => {
                    const file = draft.data.find((f) => f.id === file_id);
                    if (file) {
                      file.status = status;
                    }
                  },
                ),
              );
            }
          }
        });
      }

      socketInstance.on("connect", onConnect);
      socketInstance.on("message", onMessage);
      socketInstance.on("update_title_dialog", onUpdateTitle);
      socketInstance.on("message_error", onMessageError);
      socketInstance.on("file_status", onFileStatus);
    });

    return () => {
      mounted = false;
      if (socket) {
        socket.off("connect");
        socket.off("message");
        socket.off("update_title_dialog");
        socket.off("message_error");
        socket.off("file_status");
      }
    };
  }, [dispatch]);

  return { handleSubmit };
};
