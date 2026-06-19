import { PropsWithChildren } from "react";
import Collapse from "@mui/material/Collapse";
import { CollapseComponentProps } from "../typings/component.props";

export const CommonCollapse = ({
  open,
  children,
}: PropsWithChildren<CollapseComponentProps>) => {
  return (
    <Collapse in={open} timeout="auto" unmountOnExit>
      {children}
    </Collapse>
  );
};
