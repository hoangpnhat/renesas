/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import React, {
  PropsWithChildren,
  useCallback,
  useEffect,
  useMemo,
  useRef,
  useState,
} from "react";
import {
  BoxChatSectionProps,
  HistoryChatItem,
  MessageType,
} from "../../../typings/component.props";
import { ChatContentComponent } from "./ChatSectionContent";
import { CommonInputComponent } from "../../../components/CommonInput";
import {
  copyToClip,
  isLastEle,
  isLatestEle,
  makeBoldMarkdown,
  transformArrayToObjectBoolean,
  transformObjectBooleanToArray,
} from "../../../utils/utilities";
import { ContentsDialogResponseBody, Message } from "../../../typings/request";
import { getDialogContent } from "../../requests/chat-request";
import { AxiosResponse } from "axios";
import { useInfinityScroll } from "../../../hooks/useInfinityScroll";
import { TitleContent } from "./TitleContent";
import { useDispatch, useSelector } from "react-redux";
import { RootState } from "../../../store";
import {
  updateAllNewContent,
  updateChatContent,
} from "../../../store/reducers/chat.slice";
import { updateCommon } from "../../../store/reducers/common.slice";
import { useSubmitChat } from "../../../hooks/useSubmitChat";
import {
  removeInput,
  updateText,
  updateUserOption,
} from "../../../store/reducers/input.slice";
import { useChangeParams } from "../../../hooks/useChangeParams";
import { useMountTime } from "../../../hooks/useMountTime";
import { useNavigate } from "react-router-dom";
import { useChatToolBarItem } from "../hooks/useChatToolbarItem";
import Typography from "@mui/material/Typography";
import { CommonMenuWithIcon } from "../../../components/CommonMenuWithIcon";
import { checkBoxTypeDoc, menuContextChat } from "./chat.models";
import { AnchorContext, useMenuRef } from "../../../hooks/useMenuRef";
import { BaseActionMenu } from "../../../typings";
import { useSelectionText } from "../../../hooks/useSelectionText";
import { ChatSkeleton } from "./ChatSkeleton";
import { useTranslation } from "react-i18next";

import {
  CheckboxLabels,
  CommonInterface,
} from "../../../components/CommonFormGroup.tsx";
import { MAX_ROW_INPUT, MIN_ROW_INPUT } from "../../../constants/context.ts";

// Todo:
// [ ]: Add lazy loading for this box chat
const BoxChatSectionComponent = ({
  handleSubmit,
}: PropsWithChildren<Partial<BoxChatSectionProps>>) => {
  const chatContent = useSelector((state: RootState) => state.chat);
  const commonContext = useSelector((state: RootState) => state.common);
  const { lastPostElementRef, page, totalPage, setTotalPage, setPage } =
    useInfinityScroll();
  const resetPageBreak = useCallback(() => {
    setPage(0);
    setTotalPage(0);
    dispatch(removeInput());
  }, []);
  const [checkedState, setCheckedState] = useState<CommonInterface>(
    transformArrayToObjectBoolean(checkBoxTypeDoc, "value", true),
  );
  const { t } = useTranslation();
  const { dialog_id, selectedDialog } = useChangeParams(resetPageBreak);
  const { handleClick, handleClose, anchorEl } = useMenuRef("anchorPosition");
  const { selectedText, containerRef } = useSelectionText(
    handleClick,
    handleClose,
  );
  const actionRef: BaseActionMenu<string | HistoryChatItem> = useMemo(() => {
    return {
      explain: (model) => {
        handleSubmitChat({
          content: `${t("text.explainMoreAbout")}:\n${makeBoldMarkdown(model as string)}`,
          retrieval_place: transformObjectBooleanToArray(checkedState),
        });
      },
      copy: (model) => {
        copyToClip(model as string, () => {
          dispatch(
            updateCommon({
              snackBar: { isOpen: true, message: "Copied", type: "info" },
            }),
          );
        });
      },
    };
  }, []);
  const mountTime = useMountTime();
  const navigate = useNavigate();
  const dispatch = useDispatch();
  const { handleSubmitChat } = useSubmitChat(
    handleSubmit as (message: Message) => void,
  );

  const lastEle = useRef<HTMLDivElement>(null);
  const input = useSelector((state: RootState) => state.input);
  const handleOnChangeInput = (message: string) => {
    dispatch(updateText(message));
  };
  const loadMessageContent = (
    dialog_id: string,
    page: number,
    total_page: number,
  ) => {
    if (page <= total_page) {
      getDialogContent(dialog_id as string, {
        page: page as number,
      })
        .then((resp: AxiosResponse<ContentsDialogResponseBody>) => {
          const { results, metadata } = resp.data;
          const newData = {
            data: results as MessageType[],
          };
          setTotalPage(
            metadata.total_page - 1 >= 0 ? metadata.total_page - 1 : 0,
          );
          dispatch(
            page === 0
              ? updateAllNewContent(newData)
              : updateChatContent(newData),
          );
        })
        // eslint-disable-next-line @typescript-eslint/no-unused-vars
        .catch((_err) => {
          navigate("/chat/undefined");
        });
    }
  };
  const loadMorePosts = (dialog_id: string) => {
    if (dialog_id !== "undefined") {
      dispatch(updateCommon({ isLoading: true }));
      // console.log(commonContext);
      loadMessageContent(
        dialog_id as string,
        page as number,
        totalPage as number,
      );
      dispatch(updateCommon({ isLoading: false }));
    }
  };

  useEffect(() => {
    if (
      page >= 0 &&
      dialog_id !== "undefined" &&
      mountTime > 0 &&
      !commonContext.isThinkingChatBot
    ) {
      loadMorePosts(dialog_id as string);
    }
  }, [page, dialog_id, mountTime]);

  useEffect(() => {
    lastEle.current?.scrollIntoView({ behavior: "smooth" });
  }, [commonContext.isThinkingChatBot, dialog_id]);

  const handleChangeInputAction = async (content: string) => {
    await handleSubmitChat({
      content,
      retrieval_place: transformObjectBooleanToArray(checkedState),
    });
  };
  const onChangeLabel = (newValue: CommonInterface) => {
    dispatch(updateUserOption(newValue));
    setCheckedState({ ...checkedState, ...newValue });
  };
  const { toolbarArr } = useChatToolBarItem();
  return (
    <React.Fragment>
      <TitleContent
        selectedItem={selectedDialog as HistoryChatItem}
      ></TitleContent>
      <div
        ref={containerRef}
        className="max-h-[80%] h-[80%] flex-1 overflow-hidden w-full items-center"
      >
        <div className="w-full h-full justify-center flex shadow-box">
          <div className="w-[98%] py-2 flex flex-col-reverse overflow-y-auto max-h-full">
            {chatContent.data.length > 0 || !commonContext.isLoading ? (
              chatContent.data.map((message, index) => (
                <ChatContentComponent
                  order={index}
                  ref={
                    isLastEle(chatContent.data, index)
                      ? lastPostElementRef
                      : isLatestEle(index)
                        ? lastEle
                        : null
                  }
                  key={`chat-content-${index}`}
                  // cardProps={}
                  message={message}
                  toolbarArr={toolbarArr[message.role]}
                ></ChatContentComponent>
              ))
            ) : (
              <ChatSkeleton />
            )}
          </div>
        </div>
      </div>
      <div className="flex !backdrop-blur-md relative bg-transparent items-center w-full justify-center">
        <div className="md:w-[98%] w-full relative my-2 border-1 focus:border-input-focus bg-input-normal rounded-xl">
          <CommonInputComponent
            onChangeInput={handleOnChangeInput}
            value={input.inputText}
            headers={
              <CheckboxLabels
                onChangeLabel={onChangeLabel}
                checkState={checkedState}
                checkBoxItem={checkBoxTypeDoc}
              />
            }
            disabledSubmitButton={commonContext.isThinkingChatBot}
            handleSubmit={handleChangeInputAction}
            hasOptionButton={false}
            // inputBase={{ multiline: true, minRows: 3, maxRows: 5 }}
            textField={{
              rows: 2,
              variant: "standard",
              InputProps: { disableUnderline: true },
              placeholder: t("inputPlaceholder"),
              maxRows: MAX_ROW_INPUT,
              multiline: true,
              minRows: MIN_ROW_INPUT,
            }}
            isMultiline={true}
          />
          {/*<MarkdownInput />*/}
        </div>
      </div>
      <div className="py-2 flex w-full justify-center">
        <Typography
          fontSize={14}
          textAlign="center"
          fontStyle="italic"
          alignContent="center"
          className="w-[90%]"
        >
          {t("text.warningAboutChatBot")}
        </Typography>
      </div>
      <CommonMenuWithIcon
        menuItems={menuContextChat}
        handleClose={handleClose}
        selectedItem={selectedText}
        contextMenuPosition={anchorEl as AnchorContext}
        actionRef={actionRef}
        type="anchorPosition"
      />
    </React.Fragment>
  );
};
export default BoxChatSectionComponent;
