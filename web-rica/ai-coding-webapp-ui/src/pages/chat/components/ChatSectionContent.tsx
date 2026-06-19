/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import React, { forwardRef } from "react";
import {
  ChatBoxSectionProps,
  ChatToolBarItem,
  MessageStatus,
  MessageType,
} from "../../../typings/component.props";

import Avatar from "@mui/material/Avatar";
import Card from "@mui/material/Card";
import IconButton from "@mui/material/IconButton";
import Paper from "@mui/material/Paper";
import classNames from "classnames";
import { EditChatSection } from "./EditChatSection";
import EditIcon from "@mui/icons-material/Edit";
import { Typing } from "./Typings";
import { ROLE } from "../../../constants/common";
import { ChatToolBarList } from "./Toolbar";
import "./chat.scss";
import { CommonMarkdown } from "../../../components/CommonMarkdown";
import StarIcon from "@mui/icons-material/Star";
import { useDispatch, useSelector } from "react-redux";
import { updateContent } from "../../../store/reducers/chat.slice";
import { isLatestEle } from "../../../utils/utilities";
import { StyledAvatar } from "../../../components/auth/AvatarCard.tsx";
import { RootState } from "../../../store";
import BotIcon from "/image/RICA.ico";

export const ChatContentComponent = forwardRef<
  HTMLDivElement,
  ChatBoxSectionProps
>(({ message, cardProps, toolbarArr, order }, ref) => {
  const dispatch = useDispatch();
  const { avatarUrl, name } = useSelector((state: RootState) => state.user);
  const handleOnClickEditButton = () => {
    dispatch(updateContent({ status: "fulfill", id: message.id }));
  };

  return (
    <React.Fragment>
      <Paper
        ref={ref}
        elevation={0}
        component="div"
        sx={{ backgroundColor: "inherit" }}
        key={"paper " + message.id}
        className="flex flex-row  animate-fadeIn"
      >
        <div className="mx-1">
          <Avatar className="cursor-pointer !bg-background-paper">
            {message.role === ROLE.ASSISTANT ? (
              <StyledAvatar src={BotIcon} />
            ) : (
              <StyledAvatar src={avatarUrl} alt={name} />
            )}
          </Avatar>
        </div>

        {["completed", "pending", "queuing"].includes(
          message?.status as string,
        ) || !message.status ? (
          <div
            className={classNames(
              "max-w-full w-full flex my-3 group px-2 !rounded-2xl relative flex-col",
              {
                "overflow-x-auto": message.role === "assistant",
              },
            )}
          >
            <Card
              {...cardProps}
              sx={{ border: 0 }}
              className={classNames(
                "rounded-3xl relative w-fit",
                "!shadow-none text-pretty px-5 py-1.5",
                "transition-all duration-300 text-accent",
                {
                  "!bg-content-user hover:bg-slate-100 font-medium text-lg":
                    message.role === ROLE.USER,
                  "!bg-content-bot hover:bg-slate-50 font-normal":
                    message.role === ROLE.ASSISTANT,
                  "border-red-500 border": message.has_error,
                  "border-2 border-green-500": message.isEmphasized,
                  "border border-gray-200": !message.isEmphasized,
                },
              )}
            >
              {/*{!!message.vector_search && message.role === ROLE.ASSISTANT && (*/}
              {/*  <Chip*/}
              {/*    className=" left-0 static top-[5px] mb-2"*/}
              {/*    label={t(*/}
              {/*      RULE_NAME_DISPLAY[message.vector_search as RULE_NAME_TYPE],*/}
              {/*    )}*/}
              {/*    color="primary"*/}
              {/*  />*/}
              {/*)}*/}
              {message.is_favorite && (
                <StarIcon
                  sx={{ color: "#FC0" }}
                  fontSize="large"
                  className="absolute top-[5px] right-0 z-50"
                />
              )}
              {(["completed", "queuing"].includes(
                message.status as MessageStatus,
              ) ||
                !message.status) && (
                <CommonMarkdown
                  hasTyping={isLatestEle(order)}
                  message={message as MessageType}
                />
              )}
              {message.status === "pending" && <Typing />}
            </Card>
            {message.role && (
              <ChatToolBarList
                extraClassName={classNames({
                  "absolute bottom-[-20px] right-[5px]":
                    message.role === ROLE.USER,
                  invisible: !isLatestEle(order),
                })}
                messageItem={message as MessageType}
                listToolbar={toolbarArr as ChatToolBarItem[]}
                item={message as MessageType}
              />
            )}
          </div>
        ) : (
          <EditChatSection order={order} message={message} />
        )}
        {message.status === "completed" && message.role === "user" && (
          <Paper
            elevation={0}
            component="div"
            className="group-hover:flex group-hover:flex-col hidden"
          >
            <IconButton onClick={handleOnClickEditButton}>
              <EditIcon />
            </IconButton>
          </Paper>
        )}
      </Paper>
    </React.Fragment>
  );
});
