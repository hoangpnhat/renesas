/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { Chip, ChipProps } from "@mui/material";
import React, { ForwardedRef } from "react";

export interface CommonChipProps extends ChipProps {}

export const CommonChip = (props: CommonChipProps) => {
  return <Chip {...props} />;
};

export const CommonChipWithRef = React.forwardRef(
  (props: CommonChipProps, ref: ForwardedRef<HTMLDivElement>) => {
    return <Chip {...props} ref={ref} />;
  },
);
