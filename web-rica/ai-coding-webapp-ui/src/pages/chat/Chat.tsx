import React from "react";
import { useSocketAction } from "../../hooks/useSocketAction";
import { resetContent } from "../../store/reducers/chat.slice";
import { useDestroyState } from "../../hooks/useDestroyState";
import { resetChatbotThinkingState } from "../../store/reducers/common.slice";

const BoxChatSectionComponent = React.lazy(() => import("./components/BoxChat"));
const ChatComponent = () => {
  const { handleSubmit } = useSocketAction();
  useDestroyState(resetContent, resetChatbotThinkingState);

  return (
    <React.Fragment>
      <BoxChatSectionComponent handleSubmit={handleSubmit} />
    </React.Fragment>
  );
};
export default ChatComponent;
