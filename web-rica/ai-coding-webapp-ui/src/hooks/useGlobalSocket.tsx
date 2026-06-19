import { useDispatch } from "react-redux";
import {
  ContentDialogResults,
  DialogResult,
  Message,
} from "../typings/request";
import { getSocket } from "./useSocketIo";
import { updateCommon } from "../store/reducers/common.slice";
import {
  useEffect,
  useState,
  createContext,
  useContext,
  ReactNode,
} from "react";
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
import { useLocation } from "react-router-dom";

interface FileStatusMessage {
  file_id: string;
  file_name: string;
  status: FileStatus;
  total_pages?: number;
  size?: number;
  extension?: string;
  folder_path?: string;
}

interface SocketContextType {
  handleSubmit: (message: Message) => void;
}

const SocketContext = createContext<SocketContextType | null>(null);

export const useSocketContext = () => {
  const context = useContext(SocketContext);
  if (!context) {
    throw new Error(
      "useSocketContext must be used within GlobalSocketProvider",
    );
  }
  return context;
};

interface GlobalSocketProviderProps {
  children: ReactNode;
}

export const GlobalSocketProvider = ({
  children,
}: GlobalSocketProviderProps) => {
  const dispatch = useDispatch();
  const location = useLocation();
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

      socketInstance.on("connect", onConnect);
      socketInstance.on("message", onMessage);
      socketInstance.on("update_title_dialog", onUpdateTitle);
      socketInstance.on("message_error", onMessageError);
    });

    return () => {
      mounted = false;
      if (socket) {
        socket.off("connect");
        socket.off("message");
        socket.off("update_title_dialog");
        socket.off("message_error");
      }
    };
  }, [dispatch]);

  // effect for file_status on curetn page: show toast noti + update stt right after
  useEffect(() => {
    if (!socket) return;

    function onFileStatus(message: { data: FileStatusMessage }) {
      const { file_id, file_name, status, total_pages } = message.data;

      // show toast
      const toastType =
        status === FileStatus.completed
          ? "success"
          : status === FileStatus.failed
            ? "error"
            : "info";
      toast[toastType](`${file_name} processed ${status}`);

      const isOnFileScreen = location.pathname === "/management/file";

      if (isOnFileScreen) {
        // realtime update
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
                      if (total_pages) {
                        file.total_pages = total_pages;
                      }
                    }
                  },
                ),
              );
            }
          }
        });
      } else {
        // Invalidate cache
        store.dispatch(
          fileApiSlice.util.invalidateTags([{ type: "File", id: "LIST" }]),
        );
      }
    }

    socket.on("file_status", onFileStatus);

    return () => {
      socket.off("file_status", onFileStatus);
    };
  }, [socket, location.pathname]);

  return (
    <SocketContext.Provider value={{ handleSubmit }}>
      {children}
    </SocketContext.Provider>
  );
};
