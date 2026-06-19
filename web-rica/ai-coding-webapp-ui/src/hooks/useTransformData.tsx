/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { useCallback } from "react";

import get from "lodash/get";
import { RefItem } from "../components/layout/CommonCards.tsx";

export const useTransformData = <T,>({ refItems }: { refItems: RefItem[] }) => {
  const handleString = useCallback((item: T, refItem: RefItem) => {
    let newValue;
    if (refItem.getValue && typeof refItem.getValue === "function") {
      newValue = refItem.getValue(item);
    } else {
      newValue = get(item, refItem.name);
    }
    if (
      refItem.transformHandle &&
      typeof refItem.transformHandle === "function"
    ) {
      return refItem.transformHandle(newValue);
    }
    return newValue;
  }, []);
  const transformData = useCallback(
    (item: T) => {
      return item
        ? refItems.map((refItem) => {
            return {
              ...refItem,
              children: refItem.prefix ? (
                <span className="flex">
                  <strong>{refItem.prefix}</strong>&nbsp;
                  {handleString(item, refItem)}
                </span>
              ) : (
                handleString(item, refItem)
              ),
            } as RefItem;
          })
        : [];
    },
    [refItems, handleString],
  );
  return { transformData };
};
