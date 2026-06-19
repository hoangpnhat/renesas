import Divider from "@mui/material/Divider";
import Drawer from "@mui/material/Drawer";
import IconButton from "@mui/material/IconButton";
import Typography from "@mui/material/Typography";
import React from "react";
import ExitToAppIcon from "@mui/icons-material/ExitToApp";
import theme from "../layout/CustomTheme";
import { useDispatch, useSelector } from "react-redux";
import { RootState } from "../store";
import { closeDrawer } from "../store/reducers/drawer.slice";
import classNames from "classnames";

export const CommonDrawerComponent = () => {
  const dispatch = useDispatch();
  const drawerContext = useSelector((state: RootState) => state.drawer);
  const onClose = () => {
    if (typeof drawerContext.onClose === "function") {
      drawerContext.onClose();
    }
    dispatch(closeDrawer());
  };
  return (
    <React.Fragment>
      <Drawer
        className="box-border max-w-[60rem]"
        color="secondary"
        sx={{
          flexShrink: 0,
          // minWidth:drawerContext.isOpen ? drawerContext.drawerWidth : "16rem",
          width: drawerContext.isOpen ? drawerContext.drawerWidth : 0,
          maxWidth: drawerContext.maxDrawerWidth,
          position: drawerContext?.position,
          height: "100%",
          "& .MuiDrawer-paper": {
            width: drawerContext.drawerWidth,
            maxWidth: drawerContext.maxDrawerWidth,
            minWidth: "16rem",
            padding: theme.spacing(1),
            backgroundColor: theme.palette.background.paper,
            boxSizing: "border-box",
            overflow: "hidden",
          },
        }}
        open={drawerContext.isOpen}
        anchor={drawerContext.anchor}
        variant="persistent"
      >
        <div className="w-full flex justify-between">
          <Typography>
            <span className="font-bold text-center flex items-center ml-2 text-xl">
              {drawerContext.title}
            </span>
          </Typography>
          <IconButton onClick={onClose}>
            <ExitToAppIcon />
          </IconButton>
        </div>
        <Divider />
        <div
          className={classNames("p-2 h-full", {
            "overflow-auto": drawerContext.isOverflow,
          })}
        >
          {drawerContext.children}
        </div>
      </Drawer>
    </React.Fragment>
  );
};
