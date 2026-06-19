/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { ChangeEvent, MouseEvent, useCallback, useEffect, useMemo, useState } from "react";
import { useSearchParams } from "react-router-dom";
import { ENTRIES_STANDARD_RECORDS } from "../constants/common.ts";
import useDebounceCallback from "./useDebounceCallback.tsx";

export const useCustomPagination = (
  initialPage: number = 0,
  initialCountPage: number = ENTRIES_STANDARD_RECORDS,
  initialQuery: string = "",
) => {
  // const navigate = useNavigate();
  const [searchParams, setSearchParams] = useSearchParams();
  const [trueQuery, setTrueQuery] = useState("");
  const page = useMemo(() => {
    return Number(searchParams.get("page"));
  }, [searchParams]);
  const entries = useMemo(() => {
    return Number(searchParams.get("entries"));
  }, [searchParams]);
  const query = useMemo(() => {
    setTrueQuery(searchParams?.get("query") || "");
    return searchParams.get("query");
  }, [searchParams]);

  useEffect(() => {
    if (typeof page === "undefined" || !entries) {
      setSearchParams({
        page: String(initialPage),
        entries: String(initialCountPage),
        query: initialQuery,
      });
    }
  }, [
    initialCountPage,
    initialPage,
    setSearchParams,
    page,
    entries,
    initialQuery,
  ]);
  const handleChangeInput = useDebounceCallback(
    (event: React.ChangeEvent<HTMLInputElement>) => {
      event.preventDefault();
      setSearchParams({
        page: String(initialPage),
        entries: String(entries),
        query: event.target.value,
      });
    },
    500,
  );
  const onChangeInput = useCallback(
    (event: React.ChangeEvent<HTMLInputElement>) => {
      setTrueQuery(event.target.value);
      handleChangeInput(event);
    },
    [handleChangeInput, setTrueQuery],
  );
  const handleChangePage = useCallback(
    (event: MouseEvent<HTMLButtonElement> | null, newPage: number) => {
      event?.preventDefault();
      setSearchParams({
        page: String(newPage),
        entries: String(entries),
      });
    },
    [entries, setSearchParams],
  );

  const handleChangeRowsPerPage = useCallback(
    (event: ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
      event.preventDefault();
      setSearchParams({
        page: String(page),
        entries: event.target.value,
      });
    },
    [page, setSearchParams],
  );

  return {
    page,
    entries,
    handleChangePage,
    handleChangeRowsPerPage,
    onChangeInput,
    query,
    trueQuery,
  };
};
