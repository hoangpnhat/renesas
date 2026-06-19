import Box from "@mui/material/Box";
import LinearProgress from "@mui/material/LinearProgress";
import { CommonLoadingProps } from "../typings/component.props";
import { Fragment } from "react/jsx-runtime";

export default function LinearIndeterminate({ isLoading }: CommonLoadingProps) {
  return (
    <Fragment>
      {isLoading && (
        <Box sx={{ width: "100%" }}>
          <LinearProgress />
        </Box>
      )}
    </Fragment>
  );
}
