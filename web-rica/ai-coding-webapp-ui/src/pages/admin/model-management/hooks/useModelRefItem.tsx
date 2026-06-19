/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */
import { EditOutlined } from "@mui/icons-material";
import DeleteOutlineOutlinedIcon from "@mui/icons-material/DeleteOutlineOutlined";
import { useCallback, useMemo } from "react";
import { useTranslation } from "react-i18next";
import { useDispatch } from "react-redux";

import {
  modelAPISlice,
  ModelManagement,
  useDeleteModelByIdMutation,
} from "../../../../store/api-slice/modelAPISlice.ts";
import { AppDispatch } from "../../../../store";
import useMultiScreenDetection from "../../../../hooks/useMultiScreenDetection.tsx";
import { limitPageReturn, textTruncate } from "../../../../utils/utilities.ts";
import {
  CommonActionsProps,
  Language,
} from "../../../../typings/common.props.ts";
import { updateDialogConfig } from "../../../../store/reducers/dialogComponent.slice.ts";
import { ModelManagementEditView } from "../components/ModelManagementEditView.tsx";
import { updateCommon } from "../../../../store/reducers/common.slice.ts";
import { RefItem } from "../../../../components/layout/CommonCards.tsx";
import { normalizeDateString } from "../../../../utils/dayjs.ts";
import { Tooltip } from "@mui/material";

export const useModelRefItem = () => {
  const dispatch = useDispatch<AppDispatch>();
  const { t, i18n } = useTranslation();
  const dimensionData = useMultiScreenDetection();
  const [deleteModelById] = useDeleteModelByIdMutation();

  const deleteTopic = useCallback(
    async (id: string) => {
      deleteModelById(id)
        .unwrap()
        .then(() => {
          dispatch(
            modelAPISlice.util.updateQueryData(
              "getModels",
              {
                entries: limitPageReturn(dimensionData?.currentScreen?.width),
                query: "",
              },
              (draft) => ({
                ...draft,
                data: draft.data.filter((item) => item.id !== id),

                total: draft.total - 1,
              }),
            ),
          );
        })
        .catch(() => {
          dispatch(
            updateCommon({
              snackBar: {
                isOpen: true,
                message: "Failed to delete topic",
                type: "error",
              },
            }),
          );
        });
    },
    [deleteModelById, dimensionData?.currentScreen?.width, dispatch],
  );

  const editActions: CommonActionsProps<ModelManagement>[] = useMemo(() => {
    return [
      {
        text: t("Edit"),
        isDisabled: false,
        actionHandle: (event, item) => {
          event.preventDefault();
          if (item) {
            dispatch(
              updateDialogConfig({
                isOpen: true,
                title: t("Update Model"),
                fullWidth: false,
                hasConfirmButton: true,
                hasCancelButton: true,
                size: {
                  minHeight: 400,
                  maxHeight: "80vh",
                  minWidth: 800,
                },
                hasAction: true,
                typeButton: "submit",
                formNameSubmitButton: "common-form",
                children: <ModelManagementEditView modelId={item.id} />,
              }),
            );
          } else {
            dispatch(
              updateCommon({
                snackBar: {
                  isOpen: true,
                  message: "Cannot open the dialog",
                  type: "error",
                },
              }),
            );
          }
        },
        type: "icon-button",
        icon: <EditOutlined />,
      },
      {
        text: t("Delete"),
        isDisabled: false,
        actionHandle: (event, item) => {
          event.preventDefault();
          if (item) {
            dispatch(
              updateDialogConfig({
                isOpen: true,
                title: t("Delete Model"),
                fullWidth: false,
                hasConfirmButton: true,
                hasCancelButton: true,
                size: {
                  height: 75,
                  width: 400,
                },
                hasAction: true,
                typeButton: "button",
                children: (
                  <span>
                    {t("Are you sure you want to delete this model?")}
                  </span>
                ),
                onSubmit: async () => {
                  await deleteTopic((item as ModelManagement).id);
                  dispatch(
                    updateDialogConfig({
                      isOpen: false,
                    }),
                  );
                },
              }),
            );
          } else {
            dispatch(
              updateCommon({
                snackBar: {
                  isOpen: true,
                  message: "Cannot open the dialog",
                  type: "error",
                },
              }),
            );
          }
        },
        type: "icon-button",
        icon: <DeleteOutlineOutlinedIcon />,
      },
    ];
  }, [dispatch, t, deleteTopic]);

  const refItems: RefItem[] = useMemo(() => {
    return [
      {
        name: "name",
        variant: "h5",
        component: "div",
        color: "primary",
        fontSize: "1rem",
        gutterBottom: true,
        transformHandle: (item) => (
          <Tooltip title={item as string} placement={"top"} arrow={true}>
            <span>{textTruncate(item as string, 20)}</span>
          </Tooltip>
        ),
      },
      {
        name: "model_path",
        variant: "body2",
        sx: { color: "secondary", fontSize: "10px" },
        prefix: `${t("text.model.modelPath")}:`,
        transformHandle: (item) =>
          item && (
            <Tooltip title={item}>
              <span>{textTruncate(item as string, 10)}</span>
            </Tooltip>
          ),
      },
      {
        name: "roles",
        variant: "body2",
        sx: { color: "secondary", fontSize: "10px" },
        prefix: `${t("text.model.roles")}:`,
        transformHandle: (item) => {
          if (Array.isArray(item)) {
            const roles = item.join(", ");
            return roles ? (
              <Tooltip title={roles}>
                <span>{textTruncate(roles, 10)}</span>
              </Tooltip>
            ) : (
              "-"
            );
          }
          return "-";
        },
      },
      {
        name: "threshold",
        variant: "body2",
        sx: { color: "secondary", fontSize: "10px" },
        prefix: `${t("text.model.label.threshold")}:`,
        transformHandle: (item) => (item != null ? String(item) : "—"),
      },
      {
        name: "consumption_range_hours",
        variant: "body2",
        sx: { color: "secondary", fontSize: "10px" },
        prefix: `${t("text.model.label.consumption_range_hours")}:`,
        transformHandle: (item) => (item != null ? String(item) : "—"),
      },
      {
        name: "created_at",
        variant: "body2",
        sx: { color: "secondary", fontSize: "10px" },
        prefix: `${t("text.model.createdAt")}:`,
        transformHandle: (item) =>
          normalizeDateString(item as string, true, i18n.language as Language),
      },
    ];
  }, [t, i18n.language]);

  return { refItems, editActions };
};
