/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { Dispatch, SetStateAction, useCallback, useMemo } from "react";
import { GetBaseResponseBody } from "../typings/request.ts";

export const useGetMore = <T,>({
  items,
  setPage,
}: {
  items: GetBaseResponseBody<T>;
  setPage: Dispatch<SetStateAction<number>>;
}) => {
  const hasMore = useMemo(() => {
    return items && items?.data?.length < items?.total;
  }, [items]);
  const loadMore = useCallback(() => {
    setPage((prevPage) => prevPage + 1);
  }, [setPage]);
  return { hasMore, loadMore, setPage };
};
