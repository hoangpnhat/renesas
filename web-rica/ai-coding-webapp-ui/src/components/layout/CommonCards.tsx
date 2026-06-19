/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */
// Generate card view from material ui
import Card from "@mui/material/Card";

import Button from "@mui/material/Button";
import CardActions from "@mui/material/CardActions";
import CardContent from "@mui/material/CardContent";
import type { TypographyProps } from "@mui/material/Typography";
import Typography from "@mui/material/Typography";
import { CommonActionsProps } from "../../typings/common.props.ts";
import { CommonButtonWithIcon } from "../button/CommonButtonWithIcon.tsx";
import { ReactNode } from "react";
import { CommonType } from "../../typings";

export interface CommonCardsProps<T> {
  contentCards: RefItem[];
  actions: CommonActionsProps<T>[];
  item: T;
}

export interface RefItem extends TypographyProps {
  name: string;
  prefix?: string;
  transformHandle?: (item: CommonType) => string | ReactNode;
  getValue?: (item: unknown) => string;
}

export default function CommonCards<T>({
  contentCards,
  actions,
  item,
}: CommonCardsProps<T>) {
  return (
    <Card>
      <CardContent>
        {contentCards.map(
          (
            // eslint-disable-next-line @typescript-eslint/no-unused-vars
            { getValue, transformHandle, prefix, ...card },
            index,
          ) => (
            <Typography key={index} {...card} />
          ),
        )}
      </CardContent>
      <CardActions>
        {actions?.map((action, ind) =>
          action.type === "text" ? (
            <Button
              key={`action-${ind}`}
              onClick={(ev) => action.actionHandle(ev, item)}
              disabled={action?.disabledFunc?.(item) || false}
            >
              {action.text}
            </Button>
          ) : (
            <CommonButtonWithIcon
              key={`action-icon-${ind}`}
              onClick={(ev) => action.actionHandle(ev, item)}
              disabled={action?.disabledFunc?.(item) || false}
              title={action.text}
            >
              {action.icon}
            </CommonButtonWithIcon>
          ),
        )}
      </CardActions>
    </Card>
  );
}
