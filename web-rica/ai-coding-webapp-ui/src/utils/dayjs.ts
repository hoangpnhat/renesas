/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import dayjs from "dayjs";
import calendar from "dayjs/plugin/calendar";
import "dayjs/locale/ja";
import "dayjs/locale/de";
import { Language } from "../typings/common.props.ts";
import relativeTime from "dayjs/plugin/relativeTime";

import utc from "dayjs/plugin/utc";
import timezone from "dayjs/plugin/timezone";

dayjs.extend(calendar);
dayjs.extend(utc);
dayjs.extend(timezone);
dayjs.extend(relativeTime);
export const COMMON_DISPLAY_FORMAT = "MM-DD-YYYY";
export const COMMON_DISPLAY_FORMAT_WITH_TIME = {
  en: "MMM DD, YYYY . HH:mm",
  ja: "YYYY/MM/DD . HH:mm",
};
export const THIRTY_DAYS_SECONDS = 30 * 24 * 60 * 60;
export const COMMON_DISPLAY_FORMAT_WITH_HOUR_24: Record<Language, string> = {
  en: "YYYY-MM-DD · HH:mm",
  ja: "YYYY-MM-DD · HH:mm",
};
const userTimezone = dayjs.tz.guess();
const CALENDAR_CONFIGURATION = (locale: Language) => {
  switch (locale) {
    case "en":
      return {
        sameDay: "[Today]", // For today
        nextDay: "[Tomorrow]", // For tomorrow
        nextWeek: "dddd", // For the upcoming week
        lastDay: "[Yesterday] ", // For yesterday
        lastWeek: "[Last] dddd", // For the previous week
        sameElse: "MMMM D, YYYY", // For any other time
      };
    case "ja":
      return {
        sameDay: "[今日]", // For today
        nextDay: "[明日]", // For tomorrow
        nextWeek: "dddd", // For the upcoming week
        lastDay: "[昨日] ", // For yesterday
        lastWeek: "[前の] dddd", // For the previous week
        sameElse: "YYYY/MMMM/DD",
      };
  }
};

export const dateString2Dayjs = (
  dateString?: string | dayjs.Dayjs,
): dayjs.Dayjs | undefined => {
  // Handle undefined/null/empty cases
  if (!dateString) {
    return undefined;
  }

  // More thorough check for dayjs objects
  if (
    dateString &&
    typeof dateString === "object" &&
    typeof dateString.isValid === "function"
  ) {
    try {
      return dateString.isValid() ? dateString : undefined;
    } catch (error) {
      console.warn("Object looked like dayjs but isValid failed:", error);
      // Fall through to treat as string
    }
  }

  // Alternative dayjs check
  if (dayjs.isDayjs && dayjs.isDayjs(dateString)) {
    try {
      return dateString.isValid() ? dateString : undefined;
    } catch (error) {
      console.warn("dayjs.isDayjs passed but isValid failed:", error);
      // Fall through to treat as string
    }
  }

  // Convert to string and parse
  const stringValue = String(dateString);

  if (
    stringValue.trim() === "" ||
    stringValue === "undefined" ||
    stringValue === "null"
  ) {
    return undefined;
  }

  try {
    const result = dayjs(stringValue);
    return result.isValid() ? result : undefined;
  } catch (error) {
    console.error(`Error parsing date: "${stringValue}"`, error);
    return undefined;
  }
};
export const dayTimeZone = (date?: string, locale: Language = "en") => {
  dayjs.locale(locale);
  return dayjs.utc(date).tz(userTimezone);
};
export const getCalendarNameDay = (
  dateString: string,
  locale: Language = "en",
): string => {
  const format = CALENDAR_CONFIGURATION(locale);
  dayjs.locale(locale);
  return dateString2Dayjs(dateString)?.calendar(null, format) || "";
};

export const normalizeDateString = (
  dateString: string,
  useTimeZone: boolean = true,
  locale: Language = "en",
  format: Record<Language, string> = COMMON_DISPLAY_FORMAT_WITH_TIME,
): string => {
  return (
    (useTimeZone
      ? dayTimeZone(dateString, locale)
      : dateString2Dayjs(dateString)
    )?.format(format[locale] || COMMON_DISPLAY_FORMAT_WITH_TIME.en) || ""
  );
};

export const dateStringToUnix = (dateString: string): number => {
  return dateString2Dayjs(dateString)?.unix() || 0;
};

export const getCurrentTime = () => {
  return dayjs();
};

export const getTimeOfDay = () => {
  const hour = dayjs().hour();
  if (hour >= 5 && hour < 12) {
    return "morning";
  } else if (hour >= 12 && hour < 18) {
    return "afternoon";
  } else {
    return "evening";
  }
};
export const getLocale = (): string => {
  return dayjs.locale();
};

export const getMonthYearString = (
  dateString: string,
  locale: Language = "en",
): string => {
  const date = dateString2Dayjs(dateString);
  if (!date) return "";

  dayjs.locale(locale);

  switch (locale) {
    case "ja":
      // Ví dụ: "2025年9月"
      return date.format("YYYY年M月");
    case "en":
    default:
      // Ví dụ: "September 2025"
      return date.format("MMMM YYYY");
  }
};
