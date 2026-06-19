import React, { useEffect } from "react";
import { getSavedChatContent } from "../requests/chat-request";
import { SavedChatContent } from "../../typings/request";
import { useDispatch } from "react-redux";
import { updateAllNewNote } from "../../store/reducers/notes.slice";
import "./components/note.scss";

import Typography from "@mui/material/Typography";

export const MainViewNotes = () => {
  const dispatch = useDispatch();
  useEffect(() => {
    const getSavedChatContentRequest = async () => {
      const response = await getSavedChatContent({ page: 0 });
      const { metadata, results } = response.data;
      dispatch(
        updateAllNewNote({
          pageBreak: {
            page: metadata.page,
            total_page: metadata.total_page,
            total_records: metadata.all_records,
          },
          data: results as SavedChatContent[],
        }),
      );
    };
    getSavedChatContentRequest();
  }, []);

  return (
    <React.Fragment>
      {/* <div className="h-full w-full m-2">
        <div className="w-full h-[5%] text-center flex justify-center style-text">
          <h1 className="text-3xl font-bold preview">Hello</h1>
        </div>
        <NoteContainer />
        <CommonMarkDownEditor />
      </div> */}
      <Typography>
        This feature is in development. Will be applied later
      </Typography>
    </React.Fragment>
  );
};
