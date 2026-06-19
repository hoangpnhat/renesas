/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import IconButton from "@mui/material/IconButton";
import InputBase from "@mui/material/InputBase";
import Paper from "@mui/material/Paper";
import TextField from "@mui/material/TextField";
import Tooltip from "@mui/material/Tooltip";
import React, { PropsWithChildren, useState } from "react";
import BoltSharpIcon from "@mui/icons-material/BoltSharp";
import SendIcon from "@mui/icons-material/Send";
import { CommonInputProps } from "../typings/component.props";
import classNames from "classnames";
import { useTranslation } from "react-i18next";
import KeyboardArrowUpIcon from "@mui/icons-material/KeyboardArrowUp";
import ExpandMoreIcon from "@mui/icons-material/ExpandMore";
import { MAX_ROW_INPUT, MIN_ROW_INPUT } from "../constants/context.ts";

export const CommonInputComponent = ({
  inputBase,
  handleSubmit,
  value,
  onChangeInput,
  children,
  disabledSubmitButton,
  isMultiline,
  textField,
  headers,
  optionButton,
  hasOptionButton = false,
  hasSubmitButton = true,
}: PropsWithChildren<CommonInputProps>) => {
  const { t } = useTranslation();
  const [maxRows, setMaxRows] = useState<number>(
    inputBase?.maxRows as number | 3,
  );
  const [isExpandInput, setIsExpandInput] = useState<boolean>(false);
  const handleChangeInput: React.ChangeEventHandler<
    HTMLTextAreaElement | HTMLInputElement
  > = (event) => {
    const newMessage = event.target.value;
    onChangeInput?.(newMessage);
  };
  const handleKeyDown: React.KeyboardEventHandler<HTMLFormElement> = (
    event,
  ) => {
    if (event.key === "Enter" && !event.shiftKey) {
      // event.preventDefault();
      handleSubmitButton();
    }
  };
  const handleExpand = (event: React.MouseEvent) => {
    event.preventDefault();
    if (isExpandInput) {
      setMaxRows(MIN_ROW_INPUT);
    } else {
      setMaxRows(MAX_ROW_INPUT);
    }
    setIsExpandInput(!isExpandInput);
  };
  const handleSubmitButton = () => {
    if (!disabledSubmitButton) {
      if (typeof handleSubmit !== "undefined" && !!value) {
        handleSubmit(value);
      }
      onChangeInput?.("");
    }
  };
  return (
    <React.Fragment>
      <Paper
        component="form"
        elevation={0}
        className="px-4 py-2  max-w-full !bg-inherit max-h-full my-2 !shadow-none"
        onKeyDown={handleKeyDown}
      >
        {!!headers && (
          <section className="relative flex">
            {headers}
            <Tooltip
              placement="top"
              arrow
              title={!isExpandInput ? "Expand input" : "Contract the input"}
            >
              <IconButton
                onClick={handleExpand}
                className="absolute top-0 left-[30%]"
              >
                {isExpandInput ? <ExpandMoreIcon /> : <KeyboardArrowUpIcon />}
              </IconButton>
            </Tooltip>
          </section>
        )}
        <div className="flex items-center relative">
          {hasOptionButton && (
            <IconButton className="p-2" aria-label="Options">
              {optionButton || <BoltSharpIcon />}
            </IconButton>
          )}
          {!isMultiline ? (
            <InputBase
              autoFocus
              {...inputBase}
              className={classNames("ml-3 w-[96%] focus:bg-black", {
                "ml-5": hasOptionButton,
              })}
              placeholder={t("inputPlaceholder")}
              onChange={handleChangeInput}
              value={value}
              inputProps={{ "aria-label": "Input something" }}
            />
          ) : (
            <TextField
              autoFocus
              {...textField}
              value={value}
              onChange={handleChangeInput}
              className="w-full border-0"
              id="outlined-multiline-flexible"
              multiline
              rows={maxRows}
            />
          )}
          {hasSubmitButton && (
            <IconButton
              onClick={handleSubmitButton}
              type="button"
              disabled={disabledSubmitButton}
              className="p-4 absolute top-0"
              aria-label="search"
            >
              <SendIcon />
            </IconButton>
          )}
        </div>
        {children}
      </Paper>
    </React.Fragment>
  );
};
