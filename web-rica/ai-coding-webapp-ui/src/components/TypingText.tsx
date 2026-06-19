import React, { PropsWithChildren, useEffect, useRef } from "react";
import { TypingTextProps } from "../typings/component.props";
import { type } from "../utils/utilities";

import { ROLE } from "../constants/common";
import { useDispatch } from "react-redux";
import { updateCommon } from "../store/reducers/common.slice";
import { updateCompletedStatus } from "../store/reducers/chat.slice";

export const TypingText = ({
  text,
  role,
}: PropsWithChildren<TypingTextProps>) => {
  const dispatch = useDispatch();
  const ref = useRef<HTMLElement>(null);

  useEffect(() => {
    if (role === (ROLE.ASSISTANT as ROLE)) {
      type(text as string, ref.current as HTMLElement, 5, 1, () => {
        dispatch(updateCommon({ isThinkingChatBot: false }));
        dispatch(updateCompletedStatus());
      });
      // dispatch(updateStatus(false));
    } else {
      ref.current?.setHTMLUnsafe(text as string);
    }
    ``;
  }, []);
  return (
    <React.Fragment>
      <span ref={ref}></span>
    </React.Fragment>
  );
};
