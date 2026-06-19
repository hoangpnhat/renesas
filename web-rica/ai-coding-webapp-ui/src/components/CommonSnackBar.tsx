import Alert from "@mui/material/Alert";
import type {
  SnackbarCloseReason,
  SnackbarProps,
} from "@mui/material/Snackbar";
import Snackbar from "@mui/material/Snackbar";
import React from "react";
import { useDispatch, useSelector } from "react-redux";
import { RootState } from "../store";
import { closeSnackBar } from "../store/reducers/common.slice";

export const CommonSnackBar = (props: SnackbarProps) => {
  const commonContext = useSelector((state: RootState) => state.common);
  const dispatch = useDispatch();

  const handleClose = (
    _event?: React.SyntheticEvent | Event,
    reason?: SnackbarCloseReason,
  ) => {
    if (reason === "clickaway") {
      return;
    }
    dispatch(closeSnackBar());
  };
  return (
    <React.Fragment>
      <Snackbar
        open={commonContext.snackBar?.isOpen}
        autoHideDuration={2000}
        onClose={handleClose}
        {...props}
      >
        <Alert
          onClose={handleClose}
          severity={commonContext.snackBar?.type}
          variant="filled"
          sx={{ width: "100%" }}
        >
          {commonContext.snackBar?.message}
        </Alert>
      </Snackbar>
    </React.Fragment>
  );
};
