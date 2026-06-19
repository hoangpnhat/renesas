import { useEffect } from "react";
import { useDispatch } from "react-redux";

export const useDestroyState = (...dispatchReducers: any[]) => {
  const dispatch = useDispatch();
  useEffect(() => {
    return () => {
      dispatchReducers.forEach((reducer) => dispatch(reducer()));
    };
  }, []);
};
