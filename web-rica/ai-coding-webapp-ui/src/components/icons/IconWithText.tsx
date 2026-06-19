/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */
import { PropsWithChildren } from "react";
import { Tooltip } from "@mui/material";

interface CommonIconWithTextProps {
  text: string;
  title?: string;
}

export const CommonIconWithText = ({
                                     text,
                                     children,
                                     title,
                                   }: PropsWithChildren<CommonIconWithTextProps>) => {
  return (
    <Tooltip title={title || text} arrow>
      <section className="flex justify-start justify-items-center items-center">
        {children}
        <span className="mx-1">{text}</span>
      </section>
    </Tooltip>
  );
};
