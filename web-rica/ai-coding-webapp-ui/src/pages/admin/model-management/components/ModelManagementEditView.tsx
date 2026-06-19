/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import Box from "@mui/material/Box/Box";
import Collapse from "@mui/material/Collapse";
import Divider from "@mui/material/Divider";
import LinearProgress from "@mui/material/LinearProgress";
import Stack from "@mui/material/Stack";
import Typography from "@mui/material/Typography";
import TextField, { TextFieldProps } from "@mui/material/TextField";
import ExpandLessIcon from "@mui/icons-material/ExpandLess";
import ExpandMoreIcon from "@mui/icons-material/ExpandMore";
import { useTheme } from "@mui/material/styles";

import React, { Fragment, useCallback, useMemo, useState } from "react";
import { Controller, useFormContext } from "react-hook-form";
import { Path } from "react-hook-form";
import { useDispatch } from "react-redux";
import { useTranslation } from "react-i18next";

import { useModelDatum } from "../hooks/useModelDatum.tsx";
import { useModelSchema } from "../hooks/useModelSchema.tsx";
import useMultiScreenDetection from "../../../../hooks/useMultiScreenDetection.tsx";
import { AppDispatch } from "../../../../store";
import { updateDialogConfig } from "../../../../store/reducers/dialogComponent.slice.ts";
import {
  modelAPISlice,
  ModelCreateOrEdit,
  ModelCreateOrEditRequest,
  useCreateModelMutation,
  useGetModelsByIdQuery,
  useUpdateModelMutation,
  useGetModelDatabricksQuery,
  useGetModelRolesListQuery,
} from "../../../../store/api-slice/modelAPISlice.ts";
import { limitPageReturn } from "../../../../utils/utilities.ts";
import { CommonFormScreen } from "../../../../components/form/CommonFormScreen.tsx";
import { ExternalSx, FormItem } from "../../../../typings/common.props.ts";
import { BaseObjectWithInterface } from "../../../../typings";

const getTokenFieldSx = (theme: any) => ({
  width: "100%",
  margin: 0,
  "& .MuiOutlinedInput-root": {
    borderRadius: "12px",
    backgroundColor: theme.palette.background.paper,
    transition: "all 0.2s ease-in-out",
    "& fieldset": { borderColor: theme.palette.divider },
    "&:hover": {
      backgroundColor: theme.palette.action.hover,
      transform: "translateY(-1px)",
      boxShadow: "0 4px 12px rgba(0, 0, 0, 0.3)",
      "& fieldset": { borderColor: theme.palette.text.secondary },
    },
    "&.Mui-focused": {
      backgroundColor: theme.palette.background.paper,
      "& fieldset": {
        borderColor: theme.palette.primary.main,
        borderWidth: "2px",
      },
    },
    "&.Mui-error": {
      backgroundColor: "rgba(248,113,113,0.08)",
      "& fieldset": { borderColor: theme.palette.error.main },
      "&:hover fieldset": { borderColor: theme.palette.error.dark },
    },
  },
  "& .MuiInputLabel-root": {
    fontWeight: 500,
    "&.Mui-focused": { color: theme.palette.primary.main },
    "&.Mui-error": { color: theme.palette.error.main },
  },
  "& .MuiFormHelperText-root": {
    marginLeft: 1,
    fontSize: "0.875rem",
    "&.Mui-error": { color: theme.palette.error.main, fontWeight: 500 },
  },
});

interface TokenConfigSectionProps {
  items: FormItem<Partial<any>>[];
  open: boolean;
  onToggle: () => void;
  label: string;
}

const TokenConfigSection: React.FC<TokenConfigSectionProps> = ({
  items,
  open,
  onToggle,
  label,
}) => {
  const theme = useTheme();
  const {
    control,
    formState: { errors },
  } = useFormContext<ModelCreateOrEdit>();

  return (
    <Box sx={{ px: 2, pb: 1 }}>
      <Box
        onClick={onToggle}
        sx={{
          display: "flex",
          alignItems: "center",
          cursor: "pointer",
          py: 1,
          userSelect: "none",
          "&:hover": { opacity: 0.8 },
        }}
      >
        <Typography
          variant="subtitle2"
          sx={{
            fontWeight: 600,
            color: "text.secondary",
            flex: 1,
            fontSize: "0.9375rem",
          }}
        >
          {label}
        </Typography>
        {open ? (
          <ExpandLessIcon fontSize="small" />
        ) : (
          <ExpandMoreIcon fontSize="small" />
        )}
      </Box>
      <Divider />
      <Collapse in={open}>
        <Stack spacing={2} sx={{ pt: 2 }}>
          {items.map((item, index) => (
            <Controller
              key={index}
              control={control}
              name={item.name as Path<ModelCreateOrEdit>}
              render={({ field }) => (
                <TextField
                  {...(item.properties as TextFieldProps)}
                  {...field}
                  value={field.value ?? ""}
                  error={!!errors[item.name as keyof ModelCreateOrEdit]}
                  helperText={
                    errors[item.name as keyof ModelCreateOrEdit]
                      ?.message as React.ReactNode
                  }
                  margin="normal"
                  sx={getTokenFieldSx(theme)}
                />
              )}
            />
          ))}
        </Stack>
      </Collapse>
    </Box>
  );
};

interface CreateEditKnowledgeDetailProps extends ExternalSx {
  modelId?: string;
}

export const ModelManagementEditView: React.FC<
  CreateEditKnowledgeDetailProps
> = ({ externalSX, modelId }) => {
  const { t } = useTranslation();
  const [createModel] = useCreateModelMutation();
  const [updateModel] = useUpdateModelMutation();
  const dimensionData = useMultiScreenDetection();
  const { formItem, tokenConfigItems } = useModelDatum();
  const dispatch = useDispatch<AppDispatch>();
  const [tokenConfigOpen, setTokenConfigOpen] = useState(true);

  const onClose = useCallback(() => {
    dispatch(updateDialogConfig({ isOpen: false, children: null }));
  }, [dispatch]);

  const { data: topicDetail, isLoading } = useGetModelsByIdQuery(modelId!, {
    skip: !modelId,
  });

  const { data: modelDatabricksList } = useGetModelDatabricksQuery();
  const { data: modelRolesList } = useGetModelRolesListQuery();

  const initialValues = useMemo<ModelCreateOrEdit>(() => {
    // Find matching model_path from the modelDatabricksList
    const matchingModelPath = modelDatabricksList?.find(
      (item) => item.value === topicDetail?.model_path?.value,
    );

    // Find matching roles from the modelRolesList
    const matchingRoles = topicDetail?.roles
      ? topicDetail.roles
          .map((role) =>
            modelRolesList?.find((item) => item.value === role.value),
          )
          .filter((role): role is NonNullable<typeof role> => Boolean(role))
      : [];

    return {
      name: topicDetail?.name || "",
      max_tokens: topicDetail?.max_tokens || undefined,
      temperature: topicDetail?.temperature || undefined,
      context_length: topicDetail?.context_length || undefined,
      threshold: topicDetail?.threshold ?? null,
      consumption_range_hours: topicDetail?.consumption_range_hours ?? null,
      roles:
        matchingRoles.length > 0 ? matchingRoles : topicDetail?.roles || [],
      model_path: matchingModelPath ||
        topicDetail?.model_path || { name: "", value: "" },
    };
  }, [topicDetail, modelDatabricksList, modelRolesList]);

  const { schema } = useModelSchema(modelId);

  const handleSubmit = useCallback(
    async (data: ModelCreateOrEdit) => {
      try {
        const requestBody: ModelCreateOrEditRequest = {
          name: (data["name"] as string) || "No Name",
          model_path: data["model_path"].value as string,
          roles: data["roles"].map((role) => role.value) as string[],
          default_completion_options: data?.["roles"]
            ? {
                max_tokens: (data?.["max_tokens"] || 0) as number,
                temperature:
                  (data?.["temperature"] as number | undefined) || undefined,
                context_length:
                  (data?.["context_length"] as number | undefined) || undefined,
              }
            : undefined,
          threshold: (data?.["threshold"] as number | null) ?? null,
          consumption_range_hours:
            (data?.["consumption_range_hours"] as number | null) ?? null,
        };

        if (!modelId) {
          const response = await createModel(requestBody).unwrap();
          if (response?.id) {
            dispatch(
              modelAPISlice.util.updateQueryData(
                "getModels",
                {
                  page: 0,
                  entries: limitPageReturn(dimensionData?.currentScreen?.width),
                  query: "",
                },
                (draft) => {
                  return {
                    ...draft,
                    data: [response, ...draft.data],
                    total: draft.total + 1,
                  };
                },
              ),
            );
          }
        } else {
          await updateModel({
            id: modelId,
            requestBody: requestBody,
            entries: limitPageReturn(dimensionData?.currentScreen?.width),
          }).unwrap();
        }
        onClose();
      } catch (e) {
        console.error("Failed to create project:", e);
      }
    },
    [
      onClose,
      modelId,
      createModel,
      dispatch,
      dimensionData?.currentScreen?.width,
      updateModel,
    ],
  );

  return (
    <Fragment>
      <Box
        className="project-creation-section"
        sx={{
          height: "100%",
          overflowY: "auto",
          backgroundColor: (theme) => theme.palette.background.paper,
          ...externalSX,
        }}
      >
        {isLoading && <LinearProgress />}
        <CommonFormScreen<
          Partial<any>,
          ModelCreateOrEdit,
          BaseObjectWithInterface<unknown, string>
        >
          externalSX={{
            display: "flex",
            paddingY: 1,
            flexDirection: "column",
            justifyContent: "start",
          }}
          formName="common-form"
          schema={schema}
          onSubmit={handleSubmit}
          items={formItem}
          hasActions={false}
          initialValues={initialValues}
        >
          <TokenConfigSection
            items={tokenConfigItems}
            open={tokenConfigOpen}
            onToggle={() => setTokenConfigOpen((prev) => !prev)}
            label={t("text.model.label.tokenConfig")}
          />
        </CommonFormScreen>
      </Box>
    </Fragment>
  );
};
