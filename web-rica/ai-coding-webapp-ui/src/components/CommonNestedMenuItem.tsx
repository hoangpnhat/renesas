/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import ExpandLess from "@mui/icons-material/ExpandLess";
import ExpandMore from "@mui/icons-material/ExpandMore";
import {
  ListItemButton,
  ListItemIcon,
  ListItemText,
  Tooltip,
} from "@mui/material";
import Collapse from "@mui/material/Collapse/Collapse";
import { Fragment, useEffect, useState } from "react";
import { useTranslation } from "react-i18next";
import { useSelector } from "react-redux";
import { SideBarItem } from "../typings/component.props.ts";
import { RootState } from "../store";

interface CommonNestedMenuItemProps {
  item: SideBarItem;
  level: number;
  selectedKey: string;
  onSelect: (
    event: React.MouseEvent<HTMLDivElement>,
    item: SideBarItem,
  ) => void;
}

export const CommonNestedMenuItem = ({
  item,
  level = 0,
  selectedKey,
  onSelect,
}: CommonNestedMenuItemProps) => {
  const [open, setOpen] = useState(false);
  const hasChildren = item.children && item.children.length > 0;
  const { t } = useTranslation();
  const isSelectedSidebar = useSelector(
    (state: RootState) => state.common.isSelectedSidebar,
  );
  const handleClick = (event: React.MouseEvent<HTMLDivElement>) => {
    if (hasChildren) {
      setOpen(!open);
    }
    if (!hasChildren) {
      onSelect(event, item);
    }
  };
  useEffect(() => {
    if (item?.toggleKey) {
      if (
        (Array.isArray(item.toggleKey) &&
          item.toggleKey.some((key) => isSelectedSidebar.startsWith(key))) ||
        (typeof item.toggleKey === "string" &&
          isSelectedSidebar.startsWith(item.toggleKey))
      ) {
        setOpen(true);
      } else {
        setOpen(false);
      }
    }
  }, [isSelectedSidebar, item.toggleKey]);

  return (
    <Fragment>
      <Tooltip title={item?.disabledHelpText || ""} placement="right">
        <span>
          <ListItemButton
            disabled={item.disabled}
            onClick={handleClick}
            selected={item.to === selectedKey}
            className="w-full flex !justify-start p-2 text-text-primary rounded-lg group"
            sx={{ pl: level * 4 + 2 }}
          >
            {item.icon && <ListItemIcon>{item.icon}</ListItemIcon>}
            <ListItemText
              primary={t(item.name)}
              sx={
                !item?.toggleKey
                  ? {
                      "&>span.MuiListItemText-primary": {
                        fontWeight: 400,
                      },
                    }
                  : null
              }
            />
            {hasChildren && (open ? <ExpandLess /> : <ExpandMore />)}
          </ListItemButton>
        </span>
      </Tooltip>

      {hasChildren && (
        <Collapse in={open} timeout="auto">
          {item.children?.map((child) => (
            <CommonNestedMenuItem
              key={child.key}
              item={child}
              level={level + 1}
              selectedKey={selectedKey}
              onSelect={onSelect}
            />
          ))}
        </Collapse>
      )}
    </Fragment>
  );
};
