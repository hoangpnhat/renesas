/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import { useMemo } from "react";
import { HistoryChatItem, HistoryChatTransform } from "../typings/component.props";
import { dateStringToUnix, getCalendarNameDay } from "../utils/dayjs";
import { useTranslation } from "react-i18next";
import { Language } from "../typings/common.props.ts";

export const useTransformHistoryChat = (
  history: HistoryChatItem[],
  setTransformHistory?: any,
) => {
  const { i18n } = useTranslation();
  const historyChatTransform = useMemo<HistoryChatTransform>(() => {
    const result: HistoryChatTransform = {};
    // history.reverse();
    history.forEach((value) => {
      const calendarString = getCalendarNameDay(
        value.datetime as string,
        i18n.language as Language,
      );
      if (!Object.keys(result).includes(calendarString)) {
        result[calendarString] = {
          value: dateStringToUnix(value.datetime as string),
          history: [],
          isCollapse: false,
        };
      }
      result[calendarString].history?.push(value);
    });
    if (setTransformHistory) {
      setTransformHistory(result);
    }
    return result;
  }, [history, i18n.language]);
  return { historyChatTransform };
};
