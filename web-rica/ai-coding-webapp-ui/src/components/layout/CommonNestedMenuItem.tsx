/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import Collapse from "@mui/material/Collapse/Collapse";

import ListItemIcon from "@mui/material/ListItemIcon";
import ListItemText from "@mui/material/ListItemText";
import ListItemButton from "@mui/material/ListItemButton";
import ExpandLess from "@mui/icons-material/ExpandLess";
import ExpandMore from "@mui/icons-material/ExpandMore";
import { Fragment, useState } from "react";
import { SideBarItem } from "../../typings/component.props.ts";
import { useTranslation } from "react-i18next";

/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */
interface CommonNestedMenuItemProps {
  item: SideBarItem;
  level: number;
  selectedKey: string;
  isActive: boolean;
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
  isActive,
}: CommonNestedMenuItemProps) => {
  const [open, setOpen] = useState(isActive || false);
  const hasChildren = item.children && item.children.length > 0;
  const { t } = useTranslation();
  const handleClick = (event: React.MouseEvent<HTMLDivElement>) => {
    if (hasChildren) {
      setOpen(!open);
    }
    if (!hasChildren) {
      onSelect(event, item);
    }
  };

  return (
    <Fragment>
      <ListItemButton
        disabled={item.disabled}
        onClick={handleClick}
        selected={item.key === selectedKey}
        className="w-full flex !justify-start p-2 text-text-primary rounded-lg group"
        sx={{ pl: level * 4 + 2 }}
      >
        {item.icon && <ListItemIcon>{item.icon}</ListItemIcon>}
        <ListItemText primary={t(item.name)} />
        {hasChildren && (open ? <ExpandLess /> : <ExpandMore />)}
      </ListItemButton>

      {hasChildren && (
        <Collapse in={open} timeout="auto" unmountOnExit>
          {item.children?.map((child) => (
            <CommonNestedMenuItem
              key={child.key}
              item={child}
              level={level + 1}
              selectedKey={selectedKey}
              onSelect={onSelect}
              isActive={isActive}
            />
          ))}
        </Collapse>
      )}
    </Fragment>
  );
};
