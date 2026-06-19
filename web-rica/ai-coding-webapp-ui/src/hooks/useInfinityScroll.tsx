import { useCallback, useRef } from "react";
import debounce from "lodash/debounce";
import { useSelector } from "react-redux";
import { RootState } from "../store";
import { useScrollPagination } from "./useScrollPagination";
export const useInfinityScroll = () => {
  const observer = useRef<any>();
  const commonContext = useSelector((state: RootState) => state.common);
  const {page, totalPage, setPage, setTotalPage} = useScrollPagination()

  const lastPostElementRef = useCallback(
    (node: any) => {
      if (commonContext.isLoading) return;
      if (observer.current) observer.current.disconnect();

      observer.current = new IntersectionObserver(
        debounce((entries) => {
          if (
            entries[0].isIntersecting &&
            (page as number) <= (totalPage as number)
          ) {
            setPage(page+1)
          }
        }, 500)
      );

      if (node) observer.current.observe(node);
    },
    [commonContext.isLoading]
  );

  return {lastPostElementRef, setTotalPage, page, totalPage, setPage};
};
