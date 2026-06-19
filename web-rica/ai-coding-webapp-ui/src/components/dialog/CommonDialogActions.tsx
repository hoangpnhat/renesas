/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import Button from "@mui/material/Button";
import DialogActions from "@mui/material/DialogActions";
import { useTranslation } from "react-i18next";

export interface CommonDialogActionsProps {
  hasCancelButton?: boolean;
  handleOnClose?: () => void;
  isDisabledConfirmButton?: boolean;
  handleOnConfirm?: () => void;
  confirmButtonText?: string;
  cancelButtonText?: string;
  formNameSubmitButton?: string;
  hasConfirmButton?: boolean;
  typeButton?: "button" | "submit" | "reset";
}

export const CommonDialogActions = ({
                                      hasCancelButton,
                                      handleOnClose,

                                      handleOnConfirm,
                                      confirmButtonText,
                                      cancelButtonText,
                                      hasConfirmButton,
                                      formNameSubmitButton,
                                      typeButton
                                    }: CommonDialogActionsProps) => {
  const { t } = useTranslation();


  return (
    <DialogActions
      sx={{
        "& .MuiButton-root": {
          minWidth: 100,
          fontWeight: 500,
          textTransform: "none", // More modern look
          borderRadius: 1
        }
      }}
    >
      {(hasCancelButton ?? true) && (
        <Button
          variant="outlined"
          onClick={handleOnClose}
          color="inherit"
          sx={{
            borderColor: "divider",
            color: "text.secondary",
            "&:hover": {
              borderColor: "text.secondary",
              backgroundColor: "action.hover"
            }
          }}
        >
          {t(cancelButtonText || "Cancel")}
        </Button>
      )}

      {(hasConfirmButton ?? false) && (
        <Button
          variant="contained"
          color="primary"
          type={typeButton || "button"}
          autoFocus

          onClick={handleOnConfirm}
          form={formNameSubmitButton}
          sx={{
            boxShadow: 2,
            "&:hover": {
              boxShadow: 4
            }
          }}
        >
          {t(confirmButtonText || "Confirm")}
        </Button>
      )}
    </DialogActions>
  );
};
