import React, { useEffect, useState } from "react";
import { Outlet } from "react-router-dom";
import { getAllDialogChat } from "../requests/chat-request";
import { AxiosResponse } from "axios";
import { AllDialogResponseBody } from "../../typings/request";
import { HistoryChatItem } from "../../typings/component.props";
import "./chat.scss";
import { useDispatch } from "react-redux";
import { resetDialog, updateNew } from "../../store/reducers/dialog.slice";

const MainViewChatComponent = () => {
  const dispatch = useDispatch();
  const [runCount, setRunCount] = useState(1);
  useEffect(() => {
    console.info("Run in :", runCount);
    setRunCount(runCount + 1);
    getAllDialogChat()
      .then((res: AxiosResponse<AllDialogResponseBody>) => {
        const newData = res.data.results;
        if (res.data) {
          const resDialog: HistoryChatItem[] = newData.map((data) => {
            return {
              title: data.title,
              id: data.id,
              datetime: data.last_modified,
            };
          });
          dispatch(updateNew(resDialog));
        }
      })
      .catch((err) => {
        console.error("err: ", err);
      });

    return () => {
      dispatch(resetDialog());
    };
  }, []);
  return (
    <React.Fragment>
      <Outlet />
    </React.Fragment>
  );
};
export default MainViewChatComponent;
