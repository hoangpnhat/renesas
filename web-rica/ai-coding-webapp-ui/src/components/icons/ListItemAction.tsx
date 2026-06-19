import CheckCircleIcon from "@mui/icons-material/CheckCircle";
import WarningIcon from "@mui/icons-material/Warning";
import React from "react";
import { CircularProgressIcon } from "./CircularProgressIcon.tsx";
import {
  ProgressState,
  ProgressStatus,
} from "../../store/reducers/progress.slice.ts";

export interface ListItemActionProps {
  progress: ProgressState;
  value: number;
  submitStatus: ProgressStatus;
  children: JSX.Element | JSX.Element[];
}

export const ListItemAction: React.FC<ListItemActionProps> = ({
  progress,
  value,
  submitStatus,
  children,
}) => {
  if (Object.keys(progress).length === 0) {
    return children;
  }

  switch (submitStatus) {
    case "pending":
      return <CircularProgressIcon value={value} />;
    case "fulfilled":
      return <CheckCircleIcon color="success" />;
    case "error":
      return <WarningIcon color="error" />;

    default:
      return children;
  }
};
