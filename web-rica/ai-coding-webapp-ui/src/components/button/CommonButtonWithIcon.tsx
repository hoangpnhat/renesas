/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */
import { IconButtonProps, Tooltip } from "@mui/material";
import IconButton from "@mui/material/IconButton";
import { PopperProps } from "@mui/material/Popper";

interface CommonButtonWithIconProps extends IconButtonProps {
  title: string;
  placement?: PopperProps["placement"];
}

export const CommonButtonWithIcon = ({
  children,
  title,
  placement,
  ...props
}: CommonButtonWithIconProps) => {
  return (
    <Tooltip arrow={true} placement={placement || "right"} title={title}>
      <span>
        <IconButton {...props}>{children}</IconButton>
      </span>
    </Tooltip>
  );
};
