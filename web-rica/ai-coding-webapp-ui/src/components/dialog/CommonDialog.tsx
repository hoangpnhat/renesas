/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { useCallback, useMemo } from "react";
import { styled } from "@mui/material/styles";

import Dialog from "@mui/material/Dialog";
import DialogContent from "@mui/material/DialogContent";
import DialogTitle from "@mui/material/DialogTitle";
import IconButton from "@mui/material/IconButton";
import Typography from "@mui/material/Typography";

import CloseIcon from "@mui/icons-material/Close";
import { useDispatch, useSelector } from "react-redux";
import { RootState } from "../../store";
import {
  closeDialog,
  popDialog,
} from "../../store/reducers/dialogComponent.slice.ts";
import { useTranslation } from "react-i18next";
import Box from "@mui/material/Box";
import { CommonDialogActions } from "./CommonDialogActions.tsx";

// Enhanced styled dialog with better UX
const StyleDialog = styled(Dialog)(({ theme }) => ({
  "& .MuiPaper-root": {
    minWidth: "500px",
    maxHeight: "95vh",
    display: "flex",
    flexDirection: "column",
    borderRadius: theme.spacing(1.5),
    boxShadow: theme.shadows[24],
    backgroundColor: theme.palette.background.paper,
    backgroundImage: "none",
  },
  "& .MuiDialogTitle-root": {
    padding: theme.spacing(3, 3, 2, 3),
    backgroundColor: theme.palette.background.paper,
    borderBottom: `1px solid ${theme.palette.divider}`,
    flexShrink: 0,
  },
  "& .MuiDialogContent-root": {
    padding: theme.spacing(3),
    flex: 1,
    overflow: "auto",
    backgroundColor: theme.palette.background.paper,
    "&.MuiDialogContent-dividers": {
      borderTop: "none",
      borderBottom: `1px solid ${theme.palette.divider}`,
    },
  },
  "& .MuiDialogActions-root": {
    padding: theme.spacing(2, 3),
    backgroundColor: theme.palette.background.paper,
    flexShrink: 0, // Keep actions fixed
    gap: theme.spacing(1),
    justifyContent: "flex-end",
  },
}));

export const CommonDialogComponent = () => {
  const dialogComponent = useSelector(
    (state: RootState) => state.dialogComponent,
  );
  const stack = useMemo(() => {
    return dialogComponent?.componentStacks || [];
  }, [dialogComponent]);

  const { t } = useTranslation();
  const dispatch = useDispatch();

  const handleOnClose = useCallback(() => {
    if (stack.length > 0) {
      dispatch(popDialog());
    } else {
      dispatch(closeDialog());
    }
  }, [dispatch, stack]);

  const handleOnConfirm = useCallback(() => {
    const top = stack.at(-1) || dialogComponent;
    top?.onSubmit?.() || top?.handleOnConfirm?.();
    if (dialogComponent?.typeButton !== "submit") {
      handleOnClose();
    }
  }, [stack, dialogComponent, handleOnClose]);

  const renderDialogs = useMemo(() => {
    const fullStack = stack.length > 0 ? stack : [dialogComponent];

    return fullStack.map((item, index) => {
      const isTop = index === fullStack.length - 1;

      return (
        <div
          key={index}
          style={{
            display: isTop ? "contents" : "none", // Use contents for better flex behavior
          }}
        >
          {/* Fixed Header */}
          <DialogTitle
            sx={{
              position: "relative",
              fontSize: "1.25rem",
              fontWeight: 600,
              color: "text.primary",
            }}
          >
            {t(item.title || "Default Title")}

            {/* Enhanced close button */}
            {(item.hasCloseButton ?? true) && (
              <IconButton
                aria-label="close"
                onClick={handleOnClose}
                sx={{
                  position: "absolute",
                  right: 16,
                  top: "50%",
                  transform: "translateY(-50%)",
                  color: "text.secondary",
                  backgroundColor: "action.hover",
                  "&:hover": {
                    backgroundColor: "action.selected",
                    color: "text.primary",
                  },
                  width: 32,
                  height: 32,
                }}
              >
                <CloseIcon fontSize="small" />
              </IconButton>
            )}
          </DialogTitle>

          {/* Scrollable Content */}
          <DialogContent
            sx={{
              minHeight:
                item.size?.minHeight || dialogComponent?.size?.minHeight || 150,
              // Remove width constraints from content - let dialog paper handle it
            }}
            dividers
          >
            {item.children || (
              <Box
                sx={{
                  display: "flex",
                  alignItems: "center",
                  justifyContent: "center",
                  minHeight: 100,
                  color: "text.secondary",
                  fontStyle: "italic",
                }}
              >
                <Typography variant="body2">No content available</Typography>
              </Box>
            )}
          </DialogContent>

          {/* Fixed Actions */}
          {(item.hasAction ?? true) && (
            <CommonDialogActions
              typeButton={dialogComponent?.typeButton}
              hasCancelButton={item?.hasCancelButton}
              handleOnClose={handleOnClose}
              isDisabledConfirmButton={item.isDisabledConfirmButton}
              handleOnConfirm={handleOnConfirm}
              confirmButtonText={item.confirmButtonText}
              cancelButtonText={item.cancelButtonText}
              hasConfirmButton={item.hasConfirmButton}
              formNameSubmitButton={dialogComponent.formNameSubmitButton}
            />
          )}
        </div>
      );
    });
  }, [stack, dialogComponent, handleOnClose, handleOnConfirm, t]);

  return (
    <StyleDialog
      open={dialogComponent.isOpen}
      onClose={handleOnClose}
      maxWidth={stack.at(-1)?.maxWidth || dialogComponent.maxWidth}
      fullWidth={stack.at(-1)?.fullWidth || dialogComponent.fullWidth}
      aria-labelledby="customized-dialog-title"
    >
      {renderDialogs}
    </StyleDialog>
  );
};
