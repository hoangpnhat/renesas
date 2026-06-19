/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import classNames from "classnames";
import React, { PropsWithChildren, useMemo } from "react";

import IconButton from "@mui/material/IconButton";
import Tooltip from "@mui/material/Tooltip";

import { BaseObject } from "../../../typings";
import {
  ChatToolBarItem,
  ChatToolBarProps,
} from "../../../typings/component.props";
import { useTranslation } from "react-i18next";

export const ChatToolBarList = ({
  listToolbar,
  item,
  extraClassName,
}: PropsWithChildren<ChatToolBarProps>) => {
  const { t } = useTranslation();
  const toggleItems = useMemo(() => {
    const result: BaseObject = {};
    listToolbar.forEach((toolbarItem) => {
      if (!Object.keys(result).includes(toolbarItem.name)) {
        switch (toolbarItem.name) {
          case "best_response":
            result[toolbarItem.name] = (item.is_good_response as number) === 1;
            break;
          case "bad_response":
            result[toolbarItem.name] = (item.is_good_response as number) === 0;
            break;
          case "bookmark":
            result[toolbarItem.name] = item.is_favorite;
            break;
          default:
            result[toolbarItem.name] = false;
            break;
        }
      }
    });

    return result;
  }, [item]);

  const handleOnclick = (
    event: React.MouseEvent,
    toolbarItem: ChatToolBarItem,
  ) => {
    event.preventDefault();
    toolbarItem?.onClickHandle?.(event, item);
    // handleToggleItems(toolbarItem.name);
  };
  return (
    <div
      className={classNames(
        "flex justify-start group-hover:visible",
        extraClassName,
      )}
    >
      {item.status !== "pending" &&
        listToolbar.map((toolbarItem, index) => {
          return (
            <Tooltip
              arrow
              className="ml-2"
              title={t(toolbarItem.title)}
              key={index}
            >
              <IconButton
                size="small"
                className="mx-2"
                onClick={(event) => handleOnclick(event, toolbarItem)}
              >
                {toggleItems?.[toolbarItem.name]
                  ? toolbarItem.icon
                  : toolbarItem.outlineItem}
              </IconButton>
            </Tooltip>
          );
        })}
    </div>
  );
};
