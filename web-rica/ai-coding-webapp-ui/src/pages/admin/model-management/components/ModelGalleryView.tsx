/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { Fragment, useCallback, useState } from "react";
import { useDispatch } from "react-redux";
import { AppDispatch } from "../../../../store";
import useMultiScreenDetection from "../../../../hooks/useMultiScreenDetection.tsx";
import {
  ModelManagement,
  useGetModelsQuery,
} from "../../../../store/api-slice/modelAPISlice.ts";
import { limitPageReturn } from "../../../../utils/utilities.ts";
import { ENTRIES_PER_PAGE } from "../../../../constants/common.ts";
import { useGetMore } from "../../../../hooks/useGetMore.tsx";
import { GetBaseResponseBody } from "../../../../typings/request.ts";
import { useModelRefItem } from "../hooks/useModelRefItem.tsx";
import { useTranslation } from "react-i18next";
import MoreHorizIcon from "@mui/icons-material/MoreHoriz";
import { updateDialogConfig } from "../../../../store/reducers/dialogComponent.slice.ts";
import { ModelManagementEditView } from "./ModelManagementEditView.tsx";
import { useTransformData } from "../../../../hooks/useTransformData.tsx";
import Box from "@mui/material/Box";
import Grid from "@mui/material/Grid2";
import CommonCards from "../../../../components/layout/CommonCards.tsx";
import { CommonCardItemAdd } from "../../../../components/layout/CommonCardItemAdd.tsx";
import Button from "@mui/material/Button";

export default function ModelGalleryView() {
  const [page, setPage] = useState(0);
  const dispatch = useDispatch<AppDispatch>();
  const dimensionData = useMultiScreenDetection();
  const { data: models } = useGetModelsQuery(
    {
      page,
      entries: dimensionData?.currentScreen?.width
        ? limitPageReturn(dimensionData?.currentScreen?.width)
        : ENTRIES_PER_PAGE,
      query: "",
    },
    { skip: !dimensionData?.currentScreen?.width },
  );

  const { hasMore, loadMore } = useGetMore<ModelManagement>({
    items: models as GetBaseResponseBody<ModelManagement>,
    setPage,
  });
  const { refItems, editActions } = useModelRefItem();
  const { t } = useTranslation();
  const handleAddItem = useCallback(() => {
    dispatch(
      updateDialogConfig({
        isOpen: true,
        title: t("title.model.createModel"),
        fullWidth: false,
        hasCancelButton: true,
        hasConfirmButton: true,
        size: {
          maxHeight: "80vh",
          minWidth: 800,
        },
        hasAction: true,
        typeButton: "submit",
        formNameSubmitButton: "common-form",
        children: <ModelManagementEditView />,
      }),
    );
  }, [dispatch, t]);
  const { transformData } = useTransformData<ModelManagement>({ refItems });

  return (
    <Fragment>
      <Box
        sx={{
          height: "100%",
          display: "flex",
          flexDirection: "column",
          overflow: "hidden",
        }}
      >
        <Box
          sx={{
            flexGrow: 1,
            overflowY: "auto",
            overflowX: "hidden",
            padding: 2,
          }}
        >
          <Grid
            container
            spacing={2}
            sx={{
              width: "calc(100% - 16px)",
              margin: 0,
            }}
          >
            {models?.data.map((topic, index) => {
              return (
                <Grid
                  size={{ xs: 12, sm: 6, md: 4, lg: 3, xl: 2 }}
                  key={`common-cards-${index}`}
                >
                  <CommonCards<ModelManagement>
                    contentCards={transformData(topic)}
                    actions={editActions}
                    item={topic}
                  />
                </Grid>
              );
            })}
            <Grid size={{ xs: 12, sm: 6, md: 4, lg: 3, xl: 2 }}>
              <CommonCardItemAdd
                onClick={handleAddItem}
                label={t("text.label.card.addItem")}
              />
            </Grid>
          </Grid>
        </Box>

        {hasMore && (
          <Box
            sx={{
              display: "flex",
              justifyContent: "center",
              padding: 2,
              borderTop: "1px solid rgba(255,255,255,0.12)",
              backgroundColor: "inherit",
            }}
          >
            <Button
              variant="outlined"
              startIcon={<MoreHorizIcon />}
              onClick={loadMore}
              sx={{
                borderRadius: "24px",
                px: 3,
                py: 1,
                textTransform: "none",
                color: "#7875E8",
                backgroundColor: "rgba(91,89,211,0.12)",
                borderColor: "rgba(91,89,211,0.4)",
                "&:hover": {
                  backgroundColor: "rgba(91,89,211,0.22)",
                  borderColor: "rgba(91,89,211,0.6)",
                },
                transition: "all 0.2s ease-in-out",
                animation: "pulse 2s cubic-bezier(0.4, 0, 0.6, 1) infinite",
                "@keyframes pulse": {
                  "0%, 100%": {
                    opacity: 1,
                  },
                  "50%": {
                    opacity: 0.8,
                  },
                },
              }}
            >
              {t("text.button.loadMore")}
            </Button>
          </Box>
        )}
      </Box>
    </Fragment>
  );
}
