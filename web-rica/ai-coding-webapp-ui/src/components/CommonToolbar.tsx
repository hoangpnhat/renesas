/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */
import { Fragment } from "react";
import Box from "@mui/material/Box/Box";
import {
  ButtonProps,
  TextField,
  TextFieldProps,
  Typography,
} from "@mui/material";
import Button from "@mui/material/Button";
import { ExternalSx, PropertiesType } from "../typings/common.props.ts";
import Tooltip from "@mui/material/Tooltip";
import { useTranslation } from "react-i18next";

type ToolBarItemType = "input" | "button";

export interface ToolbarItems<T> {
  name: string;
  type: ToolBarItemType;
  properties: PropertiesType<T>;
  label: string;
}

interface CommonToolbarProps<T> extends ExternalSx {
  items: ToolbarItems<T>[];
  title: string;
  isUploadEnabled?: boolean;
}

export const CommonToolbar = <T,>({
  items,
  isUploadEnabled,
  externalSX,
  title,
}: CommonToolbarProps<T>) => {
  const { t } = useTranslation();
  return (
    <Fragment>
      <Box sx={{ ...externalSX }}>
        <Typography
          variant="subtitle2"
          component="h2"
          sx={{
            display: "flex",
            alignItems: "center",
            fontWeight: 600,
            fontSize: 25,
            marginBottom: "1em",
            marginLeft: ".6em",
          }}
        >
          {t(title)}
        </Typography>
        <section className=" flex mt-2 mx-3 toolbar-section justify-between">
          {items.map((item, index) => {
            switch (item.type) {
              case "button":
                return (
                  <Tooltip
                    title={
                      !isUploadEnabled
                        ? t("title.tooltip.serviceWarmingUp")
                        : ""
                    }
                    arrow
                    placement="top"
                    key={index}
                  >
                    <span>
                      <Button
                        key={index}
                        {...(item.properties as ButtonProps)}
                        sx={(theme) => ({
                          background: theme.palette.primary.main,
                          color: theme.palette.common.white,
                          fontWeight: 600,
                          height: "100%",
                          borderRadius: "12px",
                          padding: "10px 15px",
                          textTransform: "none",
                          boxShadow: "0px 4px 15px rgba(0, 0, 0, 0.2)",
                          transition:
                            "background 0.3s ease, box-shadow 0.3s ease, transform 0.2s ease",
                          "&:hover": {
                            background: theme.palette.primary.light,
                            boxShadow: "0px 6px 20px rgba(0, 0, 0, 0.3)",
                          },
                          "&:focus": {
                            outline: "none",
                          },
                        })}
                      >
                        {item.label}
                      </Button>
                    </span>
                  </Tooltip>
                );

              case "input":
                return (
                  <TextField
                    key={index}
                    id="outlined-basic"
                    variant="outlined"
                    placeholder="Search..."
                    {...(item.properties as TextFieldProps)}
                    sx={(theme) => ({
                      width: "50%",
                      "& .MuiOutlinedInput-root": {
                        borderRadius: "12px",
                        backgroundColor: theme.palette.background.paper,
                        transition: "all 0.2s ease-in-out",
                        "& fieldset": {
                          borderColor: theme.palette.divider,
                        },
                        "&:hover": {
                          backgroundColor: theme.palette.action.hover,
                          transform: "translateY(-1px)",
                          boxShadow: "0 4px 12px rgba(0, 0, 0, 0.3)",
                          "& fieldset": {
                            borderColor: theme.palette.text.secondary,
                          },
                        },
                        "&.Mui-focused": {
                          backgroundColor: theme.palette.background.paper,
                          "& fieldset": {
                            borderColor: theme.palette.primary.main,
                            borderWidth: "2px",
                          },
                        },
                        "&.Mui-error": {
                          "& fieldset": {
                            borderColor: theme.palette.error.main,
                          },
                          "&:hover fieldset": {
                            borderColor: theme.palette.error.dark,
                          },
                        },
                      },
                      "& .MuiInputLabel-root": {
                        fontWeight: 500,
                        "&.Mui-focused": {
                          color: theme.palette.primary.main,
                        },
                        "&.Mui-error": {
                          color: theme.palette.error.main,
                        },
                      },
                      "& .MuiFormHelperText-root": {
                        marginLeft: 1,
                        fontSize: "0.875rem",
                        "&.Mui-error": {
                          color: theme.palette.error.main,
                          fontWeight: 500,
                        },
                      },
                    })}
                  />
                );
              default:
                return <Fragment></Fragment>;
            }
          })}
        </section>
      </Box>
    </Fragment>
  );
};
