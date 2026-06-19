import { useEffect, useState } from "react";
import { useDispatch, useSelector } from "react-redux";
import { useLocation, useParams } from "react-router-dom";
import { updateCommon } from "../store/reducers/common.slice";
import { RootState } from "../store";
import { BUTTON_VALUE_NAME } from "../constants/common";
import { HistoryChatItem } from "../typings/component.props";
import { resetContent } from "../store/reducers/chat.slice";
import { setTitleProject } from "../utils/utilities";
import { removeInput } from "../store/reducers/input.slice";

export function useChangeParams<T extends () => void>(resetPage: T) {
  const dispatch = useDispatch();
  const location = useLocation();
  const { dialog_id } = useParams();
  const [selectedDialog, setSelectedDialog] = useState<HistoryChatItem>();
  const commonContext = useSelector((state: RootState) => state.common);
  const dialogContext = useSelector((state: RootState) => state.dialog);
  useEffect(() => {
    if (!!dialog_id && dialog_id !== "undefined" && dialogContext.length > 0) {
      setSelectedDialog(
        dialogContext.find((chat) => chat.id === dialog_id) as HistoryChatItem,
      );
    } else {
      setSelectedDialog({
        id: "",
        title: "New Chat",
        datetime: "",
      });
    }
  }, [dialog_id, dialogContext]);
  useEffect(() => {
    resetPage();
    if (!commonContext.isThinkingChatBot) {
      dispatch(resetContent());
    }
    dispatch(removeInput());
    if (!!dialog_id && dialog_id !== "undefined") {
      dispatch(
        updateCommon({ isSelectedSidebar: dialog_id, isShowingSideBar: true }),
      );
    } else {
      dispatch(
        updateCommon({
          isSelectedSidebar: BUTTON_VALUE_NAME.ADD_NEW_DIALOG,
          isShowingSideBar: false,
        }),
      );
    }

    if (location.pathname.includes("chat")) {
      dispatch(updateCommon({ isShowingSideBar: true }));
    } else {
      dispatch(updateCommon({ isShowingSideBar: false }));
    }
  }, [dialog_id, location]);
  useEffect(() => {
    document.title = setTitleProject(selectedDialog?.title || "");
  }, [selectedDialog]);
  return { dialog_id, selectedDialog };
}
