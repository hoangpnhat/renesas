/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import { GET_FILE_FROM_URL_REGEX, URL_VALIDATION } from "../constants/regex";
import { environment } from "../environments/environment.dev";
import { BaseObject } from "../typings";
import { CommonInterface } from "../components/CommonFormGroup.tsx";
import {
  ENTRIES_PER_PAGE,
  ENTRIES_PER_PAGE_SMALL,
  WIDTH_THRESHOLD,
} from "../constants/common.ts";
import {
  SecondaryText,
  SelectItem,
} from "../components/form/CommonSelections.tsx";
import {
  COMMON_DISPLAY_FORMAT_WITH_HOUR_24,
  normalizeDateString,
} from "./dayjs.ts";

export const randomId = (): string => {
  return Math.random().toString(36).slice(2, 9);
};
export const type = (
  word: string,
  ref: HTMLElement,
  timeout: number = 50,
  world_count = 10,
  additionalFunc: () => void,
) => {
  let j = 0;

  function typing() {
    ref.innerHTML = word.substring(0, j + world_count);
    j += world_count;
    if (j >= word.length) {
      additionalFunc();
      return;
    }
    setTimeout(typing, timeout);
  }

  typing();
};

export const isLastEle = <T>(arr: T[], index: number) => {
  return arr.length - 1 === index;
};
export const isLatestEle = (index: number) => {
  return index === 0;
};

export const getFileNameFromURL = (
  url: string,
  reg: RegExp = GET_FILE_FROM_URL_REGEX,
  index_to_return: number = 0,
): string => {
  const fileName = url.match(reg);
  if (fileName) {
    return fileName[index_to_return];
  }
  return "";
};

export function copyToClip(str: string, callBackfn?: () => void) {
  navigator.clipboard.writeText(str);
  if (callBackfn) {
    callBackfn();
  }
}

export function makeBoldMarkdown(str: string) {
  return `**${str}**`;
}

export function imageToBlob(imageFrame: Blob) {
  // const blob = await imageFrame.blob();
  return URL.createObjectURL(imageFrame);
}

export function setTitleProject(title: string) {
  return `${environment.projectName} | ${title}`;
}

export function jsonToValueSet<T extends BaseObject>(
  jsonList: T[],
  key: string,
) {
  return new Set(jsonList.map((item) => item[key]));
}

export function uniqueObjectsByKey<T extends BaseObject>(
  jsonList: T[],
  key: string,
) {
  const seen = new Set();
  return new Set(
    jsonList.filter((item) => {
      const value = item[key];
      if (!value || seen.has(value)) {
        return false;
      }
      seen.add(value);
      return true;
    }),
  );
}

export function transformObjectBooleanToArray(
  object: CommonInterface,
): string[] {
  return Object.keys(object).reduce((previousValue, currentValue) => {
    if (object[currentValue]) {
      return [...previousValue, currentValue];
    }
    return previousValue;
  }, [] as string[]);
}

export function transformArrayToObjectBoolean<T>(
  arr: T[],
  keyMap: keyof T,
  default_value = false,
) {
  return arr.reduce((previous, curr) => {
    return { ...previous, [curr[keyMap] as string]: default_value };
  }, {} as CommonInterface);
}

export function removeMultiple<T>(
  arrayToRemove: T[],
  keyMap: string[],
  fieldString: keyof T,
): T[] {
  return arrayToRemove.filter(
    (item) => !keyMap.includes(item[fieldString] as string),
  );
}

export function createQueryParams(
  baseUrl: string,
  params: { [key: string]: string },
) {
  const url = new URL(baseUrl); // Create a URL object
  Object.keys(params).forEach((key) =>
    url.searchParams.append(key, params[key]),
  );
  return url.toString(); // Convert back to string
}

export function checkIfKeyInArr(
  arr: string[] | undefined,
  key: string | string[],
  compareFunction?: (a: string, b: string) => boolean,
): boolean {
  // Normalize inputs once
  const normalizedArr = Array.isArray(arr) ? arr : (arr ?? "").split(",");
  const normalizedKeys = Array.isArray(key) ? key : [key];

  // Early exit for empty arrays
  if (normalizedArr.length === 0 || normalizedKeys.length === 0) return false;

  // Single loop with early return - O(n*m) worst case but exits immediately on match
  for (const arrItem of normalizedArr) {
    for (const keyItem of normalizedKeys) {
      if (
        compareFunction
          ? compareFunction(arrItem, keyItem)
          : arrItem.startsWith(keyItem)
      ) {
        return true;
      }
    }
  }
  return false;
}

export function isEqual<T, U>(
  a: T,
  b: U,
  visited: Map<unknown, unknown> = new Map(),
  strict_mode: boolean = true,
): boolean {
  // Handle identical references (including null, undefined)
  if (Object.is(a, b)) {
    return true;
  }

  // If only one is null/undefined, they're not equal
  if (a === null || a === undefined || b === null || b === undefined) {
    return false;
  }

  // Handle primitive types - already checked for equality with Object.is
  if (typeof a !== "object" || typeof b !== "object") {
    return false;
  }

  // Check for circular references
  if (visited.has(a)) {
    return visited.get(a) === b;
  }

  // Record this pair to detect circular references
  visited.set(a, b);

  // Handle different object types
  const aType = Object.prototype.toString.call(a);
  const bType = Object.prototype.toString.call(b);

  if (aType !== bType) {
    return false;
  }

  // Handle dates
  if (a instanceof Date && b instanceof Date) {
    return a.getTime() === b.getTime();
  }

  // Handle regular expressions
  if (a instanceof RegExp && b instanceof RegExp) {
    return a.toString() === b.toString();
  }

  // Handle Arrays
  if (Array.isArray(a) && Array.isArray(b)) {
    if (a.length !== b.length && strict_mode) {
      return false;
    }

    if (strict_mode) {
      for (let i = 0; i < a.length; i++) {
        if (!isEqual(a[i], b[i], new Map(visited))) {
          return false;
        }
      }
    } else {
      return b.every((bItem) =>
        a.some((aItem) => isEqual(aItem, bItem, new Map(visited), false)),
      );
    }

    return true;
  }

  // Handle Sets
  if (a instanceof Set && b instanceof Set) {
    if (a.size !== b.size) {
      return false;
    }

    // Convert to arrays and sort (not perfect but decent approximation)
    const aValues = Array.from(a);
    const bValues = Array.from(b);

    // This is simplified and may not work for complex nested structures in sets
    return isEqual(aValues, bValues, new Map(visited));
  }

  // Handle Maps
  if (a instanceof Map && b instanceof Map) {
    if (a.size !== b.size) {
      return false;
    }

    for (const [key, value] of a.entries()) {
      if (!b.has(key) || !isEqual(value, b.get(key), new Map(visited))) {
        return false;
      }
    }
    return true;
  }

  // Handle ArrayBuffers and TypedArrays
  if (ArrayBuffer.isView(a) && ArrayBuffer.isView(b)) {
    if (
      (a as ArrayBufferView).byteLength !== (b as ArrayBufferView).byteLength
    ) {
      return false;
    }

    return Array.from(a as unknown as ArrayLike<number>).every(
      (val, i) => val === Array.from(b as unknown as ArrayLike<number>)[i],
    );
  }

  // Handle plain objects
  const aKeys = Object.keys(a as object);
  const bKeys = Object.keys(b as object);

  if (aKeys.length !== bKeys.length) {
    return false;
  }

  // Check if all keys in a exist in b
  if (!aKeys.every((key) => Object.prototype.hasOwnProperty.call(b, key))) {
    return false;
  }

  // Compare each property
  return aKeys.every((key) =>
    isEqual(
      (a as Record<string, unknown>)[key],
      (b as Record<string, unknown>)[key],
      new Map(visited),
    ),
  );
}

export function transformArrayObjectToOptionArray<T>(
  oriArrOb: T[],
  mapKey: {
    name: keyof T;
    value: keyof T;
  },
  callBackDisabled?: (value: T) => boolean,
  callBackTitle?: (value: T) => string,
  callBackSecondaryText?: (value: T) => SecondaryText,
): SelectItem[] {
  return (
    oriArrOb?.map((item) => ({
      name: item[mapKey.name] as string,
      value: item[mapKey.value] as string,
      disabled: callBackDisabled ? callBackDisabled(item) : false,
      title: callBackTitle ? callBackTitle(item) : "",
      secondaryText: callBackSecondaryText && callBackSecondaryText(item),
    })) || []
  );
}

export function testReg(oriStr: string, pattern: RegExp) {
  return pattern.test(oriStr);
}

export function isURL(origin?: string): boolean {
  return !!origin && testReg(origin, URL_VALIDATION);
}

export function limitPageReturn(width?: number) {
  return !!width && width > WIDTH_THRESHOLD
    ? ENTRIES_PER_PAGE
    : ENTRIES_PER_PAGE_SMALL;
}

export function swapUsingDestructuring<T>(
  arr: T[],
  index1: number,
  index2: number,
) {
  [arr[index1], arr[index2]] = [arr[index2], arr[index1]];
  return arr;
}

export const textTruncate = (
  text: string,
  maxLength: number,
  suffix = "...",
) => {
  return text.length > maxLength ? text.slice(0, maxLength) + suffix : text;
};

export function parseBooleanStr<T>(value: T): boolean {
  if (typeof value === "boolean") {
    return value;
  }
  if (typeof value === "string") {
    const normalized = value.trim().toLowerCase();
    if (["true", "1", "yes"].includes(normalized)) {
      return true;
    }
    if (["false", "0", "no"].includes(normalized)) {
      return false;
    }
  }
  return Boolean(value);
}

export function generateOwnerString(owner: string, createdAt: string): string {
  return `Owner: ${owner} at ${normalizeDateString(
    createdAt,
    true,
    "en",
    COMMON_DISPLAY_FORMAT_WITH_HOUR_24,
  )}`;
}
