import React from "react";
import Backdrop from "@mui/material/Backdrop";
import CircularProgress from "@mui/material/CircularProgress";

export const CommonLoading = () => {
  return (
    <React.Fragment>
      <Backdrop
        open={true}
        sx={(theme) => ({
          color: "common.white",
          zIndex: theme.zIndex.drawer + 1,
        })}
      >
        <CircularProgress color="primary" size={70} />
      </Backdrop>
    </React.Fragment>
  );
};
