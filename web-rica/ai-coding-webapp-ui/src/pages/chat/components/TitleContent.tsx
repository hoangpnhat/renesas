/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import Button from "@mui/material/Button";
import React, { PropsWithChildren } from "react";
import ExpandMoreIcon from "@mui/icons-material/ExpandMore";
import { TitleContentBarProps } from "../../../typings/component.props";
import { CommonMenuWithIcon } from "../../../components/CommonMenuWithIcon";
import { useMenuRef } from "../../../hooks/useMenuRef";
import { menuItems } from "./chat.models";
import IconButton from "@mui/material/IconButton";
import Tooltip from "@mui/material/Tooltip";

import { useDispatch, useSelector } from "react-redux";
import { useDialogActions } from "../hooks/useDialogActions";

import { RootState } from "../../../store";
import { OpenSidebarIcon } from "../../../components/icons/OpenSidebarIcon.tsx";
import RICALogo from "/image/RICA_icon_medium.png";
import { updateCommon } from "../../../store/reducers/common.slice.ts";
import classNames from "classnames";

export const TitleContent = ({
  selectedItem,
}: PropsWithChildren<TitleContentBarProps>) => {
  const { handleClick, handleClose, anchorEl } = useMenuRef("anchorEl");
  const commonSlice = useSelector((state: RootState) => state.common);
  const dispatch = useDispatch();
  const onCloseSidebar = (event: React.MouseEvent) => {
    event.preventDefault();
    dispatch(updateCommon({ isShowingSideBar: true }));
  };

  const { actionRef } = useDialogActions();
  return (
    <React.Fragment>
      <div className="!w-full !backdrop-blur-lg !bg-transparent max-h-[8%] flex justify-between items-center py-2">
        <div
          className={classNames(
            "flex justify-between items-center transition-all duration-500 ease-in-out",
            {
              hidden: commonSlice.isShowingSideBar,
            },
          )}
        >
          <Tooltip title="View History" arrow>
            <IconButton onClick={onCloseSidebar}>
              <OpenSidebarIcon fontSize="medium" />
            </IconButton>
          </Tooltip>
          <img alt="RICA Logo" className="!w-[128px]" src={RICALogo} />
        </div>
        <Tooltip title={selectedItem?.title} arrow>
          <Button
            onClick={handleClick}
            sx={{
              maxWidth: "400px",
              justifyContent: "flex-start", // This aligns content to the left
              textAlign: "left",
              color: "-moz-initial",
              "& .MuiButton-startIcon": {
                marginRight: "auto",
              },
              "& .MuiButton-endIcon": {
                marginLeft: "8px",
              },
              "& .MuiButton-text": {
                // Target the text content
                textOverflow: "ellipsis",
                whiteSpace: "nowrap",
                overflow: "hidden",
                textAlign: "left",
                display: "block",
              },
            }}
          >
            <span
              style={{
                textOverflow: "ellipsis",
                whiteSpace: "nowrap",
                overflow: "hidden",
                display: "block",
              }}
            >
              {selectedItem?.title}
            </span>
            <ExpandMoreIcon />
          </Button>
        </Tooltip>

        <CommonMenuWithIcon
          menuItems={menuItems}
          handleClose={handleClose}
          selectedItem={selectedItem}
          anchorEl={anchorEl as HTMLElement}
          actionRef={actionRef}
          type="anchorEl"
        />
      </div>
    </React.Fragment>
  );
};
