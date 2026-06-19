/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { Fragment, useCallback } from "react";

import Typography from "@mui/material/Typography";

import { useTranslation } from "react-i18next";
import ModelGalleryView from "./components/ModelGalleryView.tsx";
import Box from "@mui/material/Box";
import { CommonButtonWithIcon } from "../../../components/button/CommonButtonWithIcon.tsx";
import { JSONBracesIcon } from "../../../components/icons/JSONBracesIcon.tsx";
import { useDispatch } from "react-redux";
import { updateDialogConfig } from "../../../store/reducers/dialogComponent.slice.ts";
import CommonJsonViewer from "../../../components/json-viewer/CommonJsonViewer.tsx";
import { useLazyGetModelsConfigurationQuery } from "../../../store/api-slice/modelAPISlice.ts";

const ModelMainScreen = () => {
  const { t } = useTranslation();
  const dispatch = useDispatch();
  const [getModelConfigs, { data: modelConfigs }] =
    useLazyGetModelsConfigurationQuery();
  const handleOpenModelConfig = useCallback(async () => {
    getModelConfigs()
      .unwrap()
      .then((response) => {
        dispatch(
          updateDialogConfig({
            isOpen: true,
            title: t("title.dialog.viewConfiguration") || "View Configuration",
            maxWidth: "lg",
            children: (
              <CommonJsonViewer
                initialJson={response}
                hasJsonInput={true}
                hasJsonStructure={false}
              />
            ),
            hasAction: true,
            hasConfirmButton: false,
            hasCancelButton: true,
          }),
        );
      });
  }, [t, modelConfigs, getModelConfigs]);
  return (
    <Fragment>
      <Box
        sx={{
          display: "flex",
          alignItems: "center",
          justifyContent: "space-between",
          width: "100%",
        }}
      >
        <Typography
          variant="subtitle2"
          component="h2"
          sx={{
            display: "flex",
            alignItems: "center",
            fontWeight: 500,
            fontSize: 25,
            marginLeft: "1em",
            marginBottom: "0.5em",
          }}
        >
          {t("title.model.modelManagement") || "Model Management"}
        </Typography>
        <CommonButtonWithIcon
          onClick={handleOpenModelConfig}
          title="View Configuration"
        >
          <JSONBracesIcon />
        </CommonButtonWithIcon>
      </Box>
      <ModelGalleryView />
    </Fragment>
  );
};
export default ModelMainScreen;
