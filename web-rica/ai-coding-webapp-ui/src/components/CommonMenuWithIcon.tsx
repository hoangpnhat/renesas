/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import ListItemIcon from "@mui/material/ListItemIcon";
import ListItemText from "@mui/material/ListItemText";
import type { MenuProps } from "@mui/material/Menu";
import Menu from "@mui/material/Menu";
import MenuItem from "@mui/material/MenuItem";
import Typography from "@mui/material/Typography";
import { CommonMenuProps, HistoryChatItem } from "../typings/component.props";
import { alpha, styled } from "@mui/material/styles";
import React from "react";
import { useTranslation } from "react-i18next";

export const StyledMenu = styled((props: MenuProps) => (
  <Menu
    elevation={0}
    anchorOrigin={{
      vertical: "top",
      horizontal: "left",
    }}
    transformOrigin={{
      vertical: "top",
      horizontal: "left",
    }}
    {...props}
  />
))(({ theme }) => ({
  "& .MuiPaper-root": {
    borderRadius: 6,
    marginTop: theme.spacing(1),
    minWidth: 180,
    color:
      theme.palette.mode === "light"
        ? "rgb(55, 65, 81)"
        : theme.palette.grey[300],
    boxShadow:
      "rgb(255, 255, 255) 0px 0px 0px 0px, rgba(0, 0, 0, 0.05) 0px 0px 0px 1px, rgba(0, 0, 0, 0.1) 0px 10px 15px -3px, rgba(0, 0, 0, 0.05) 0px 4px 6px -2px",
    "& .MuiMenu-list": {
      padding: "4px 0",
    },
    "& .MuiMenuItem-root": {
      "& .MuiSvgIcon-root": {
        fontSize: 18,
        color: theme.palette.text.secondary,
        marginRight: theme.spacing(1.5),
      },
      "&:active": {
        backgroundColor: alpha(
          theme.palette.primary.main,
          theme.palette.action.selectedOpacity,
        ),
      },
    },
  },
}));

export const CommonMenuWithIcon = ({
  menuItems,
  anchorEl,
  handleClose,
  selectedItem,
  actionRef,
  contextMenuPosition,
  type,
}: Partial<CommonMenuProps<HistoryChatItem | string>>) => {
  const open =
    type === "anchorEl" ? Boolean(anchorEl) : Boolean(contextMenuPosition);
  const handleClick = (event: React.MouseEvent<HTMLLIElement, MouseEvent>) => {
    event.preventDefault();

    handleClose?.();
  };
  const { t } = useTranslation();

  return (
    <Menu
      anchorEl={anchorEl}
      anchorReference={type}
      id="account-menu"
      anchorPosition={
        contextMenuPosition
          ? {
              top: contextMenuPosition.mouseY,
              left: contextMenuPosition.mouseX,
            }
          : undefined
      }
      open={open}
      onClose={handleClose}
      onClick={handleClose}
      slotProps={{
        paper: {
          elevation: 0,
          sx: {
            overflow: "visible",
            filter: "drop-shadow(0px 2px 8px rgba(0,0,0,0.32))",
            mt: 1.5,
            "&::before": {
              content: '""',
              display: "block",
              position: "absolute",
              top: 0,
              left: 14,
              width: 10,
              height: 10,
              bgcolor: "background.paper",
              transform: "translateY(-50%) rotate(45deg)",
              zIndex: 0,
            },
          },
        },
      }}
      transformOrigin={{ horizontal: "left", vertical: "top" }}
      anchorOrigin={{ horizontal: "left", vertical: "bottom" }}
    >
      {menuItems?.map((menuItem, index) => {
        return (
          <MenuItem
            key={index}
            onClick={(event) => {
              handleClick(event);
              actionRef?.[menuItem.key](selectedItem as HistoryChatItem);
            }}
          >
            <ListItemIcon>{menuItem.icon}</ListItemIcon>
            <ListItemText>{t(menuItem.text)}</ListItemText>
            {!!menuItem.shortcutText && (
              <Typography variant="body2" color="text.secondary">
                {menuItem.shortcutText}
              </Typography>
            )}
          </MenuItem>
        );
      })}
    </Menu>
  );
};
