/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */
import { yupResolver } from "@hookform/resolvers/yup";

import {
  Button,
  SelectProps,
  Stack,
  TextField,
  TextFieldProps,
  Tooltip,
  useTheme,
} from "@mui/material";
import Box from "@mui/material/Box/Box";
import { Fragment, ReactNode, useCallback, useEffect, useMemo } from "react";
import {
  Controller,
  FieldError,
  FieldValues,
  FormProvider,
  Path,
  Resolver,
  SubmitHandler,
  useForm,
} from "react-hook-form";
import { useSelector } from "react-redux";
import { ObjectSchema } from "yup";
import { RootState } from "../../store";
import { BaseObject, BaseObjectWithInterface } from "../../typings";
import {
  ExternalSx,
  FormItem,
  MultipleAutoComplete,
} from "../../typings/common.props.ts";
import { isEqual } from "../../utils/utilities.ts";
import { CommonAutoComplete } from "./CommonAutoComplete.tsx";
import { MaxCharsInput } from "./CommonMaxcharsInput.tsx";
import { CommonSelection } from "./CommonSelections.tsx";
import { CommonUploadFile } from "./CommonUploadFile.tsx";
import { CommonSingleAutoComplete } from "./CommonSingleAutoComplete.tsx";
import CommonDateTimePicker from "./CommonDateTimePicker.tsx";

interface CommonFormScreenProps<
  T,
  TType extends FieldValues,
  K extends BaseObjectWithInterface<unknown>,
> extends ExternalSx {
  items: FormItem<T, K>[];
  formName?: string;
  onSubmit: SubmitHandler<TType>;
  onCancel?: () => void;
  schema: ObjectSchema<TType>;
  hasActions?: boolean;
  primaryButtonOnClick?: () => void;
  hasCancel?: boolean;
  isSubmitting?: boolean;
  primaryButtonType?: "submit" | "button";
  hasSubmit?: boolean;
  secondaryButtonText?: string;
  resetValue?: boolean;
  isSubmittedAfterPreview?: boolean;
  setIsSubmittedAfterPreview?: (value: boolean) => void;
  initialValues?: TType;
  children?: ReactNode;
}

export const CommonFormScreen = <
  T,
  TType extends FieldValues,
  K extends BaseObject,
>({
  externalSX,
  items,
  onSubmit,
  onCancel,
  isSubmitting,
  schema,
  secondaryButtonText,
  primaryButtonOnClick,
  formName = "default",
  primaryButtonType = "submit",
  hasSubmit = true,
  hasActions = true,
  hasCancel = true,
  resetValue = true,
  isSubmittedAfterPreview = false,
  setIsSubmittedAfterPreview,
  children,
}: CommonFormScreenProps<T, TType, K>) => {
  const theme = useTheme();

  const inputSx = {
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
  };

  const methods = useForm<TType>({
    resolver: yupResolver(schema) as unknown as Resolver<TType, unknown>,
    defaultValues: schema.getDefault(),
  });
  const {
    control,
    handleSubmit,
    reset,
    watch,
    formState: { errors, isDirty },
  } = methods;
  const defaultValue = watch();

  const fileValidateStatus = useSelector(
    (state: RootState) => state.fileValidateStatus,
  );

  const isValidateFailed = useMemo(() => {
    return Object.keys(fileValidateStatus).some(
      (documentId) => fileValidateStatus[documentId].status === "failed",
    );
  }, [fileValidateStatus]);

  const internalSubmit: SubmitHandler<TType> = useCallback(
    async (data) => {
      if (onSubmit) {
        await onSubmit(data);
      }
      const newDefaults = schema.getDefault();
      if (resetValue) {
        reset(newDefaults);
      }
    },
    [onSubmit, reset, schema],
  );

  useEffect(() => {
    items.forEach((item) => {
      if (item.type === "selection") {
        if (typeof item.onChangeHandle === "function") {
          item.onChangeHandle(defaultValue[item.name as keyof TType] as string);
        }
      }
    });
  }, [items, defaultValue]);

  useEffect(() => {
    // in case we need to confirm after submit server,
    // need to reset after submit done (success/fail case)
    if (isSubmittedAfterPreview) {
      reset(schema.getDefault());
      if (setIsSubmittedAfterPreview) {
        setIsSubmittedAfterPreview(false);
      }
    }
  }, [isSubmittedAfterPreview, reset, schema]);

  useEffect(() => {
    const newDefaults = schema.getDefault();
    reset(newDefaults);
    // This updates form values when schema changes
  }, [schema, reset]);
  const isChanged = useMemo(() => {
    return isDirty && !isEqual(defaultValue, schema.getDefault());
  }, [defaultValue, schema, isDirty]);
  const handleFormReturn = useCallback(
    (item: FormItem<T, K>, index: number) => {
      switch (item.type) {
        case "upload_file":
          return (
            <CommonUploadFile
              control={control}
              key={index}
              name={item.name}
              errors={errors[item.name as keyof TType] as FieldError}
              helperText={errors[item.name as keyof TType]?.message as string}
              additionalErrorMessage={item.additionalErrorMessage}
              isMultiple={item?.isMultiple}
              customAccept={
                typeof item.customAcceptString === "string"
                  ? item.customAcceptString
                  : item.customAcceptString?.(defaultValue as unknown as K)
              }
              validateFileSize={item.validateFileSize}
            />
          );
        case "input":
          return (
            <Controller
              control={control}
              key={index}
              render={({ field }) => {
                return (
                  <TextField
                    {...(item.properties as TextFieldProps)}
                    {...field}
                    value={field.value}
                    onChange={field.onChange}
                    error={!!errors[item.name as keyof TType]}
                    helperText={
                      errors[item.name as keyof TType]
                        ?.message as React.ReactNode
                    }
                    margin="normal"
                    sx={{ width: "100%", margin: 0, ...inputSx }}
                  />
                );
              }}
              name={item.name as Path<TType>}
            />
          );
        case "max_chars_input":
          return (
            <MaxCharsInput
              key={index}
              control={control}
              name={item.name as Path<TType>}
              maxChars={item?.properties?.maxChars || 100} // Default to 100 if not specified
              error={!!errors[item.name as keyof TType]}
              helperText={
                errors[item.name as keyof TType]?.message as React.ReactNode
              }
              textFieldProps={
                item.properties as Omit<TextFieldProps, "onChange" | "value">
              }
            />
          );
        case "selection":
          return (
            <CommonSelection<T>
              key={index}
              {...(item.properties as SelectProps)}
              formItem={item as FormItem<T, BaseObjectWithInterface<unknown>>}
              label={item.label}
              variant="outlined"
              name={item.name as Path<TType>}
              control={control}
              sx={inputSx}
            />
          );
        case "single-autocomplete":
          return (
            <Controller
              key={index}
              control={control}
              render={({ field }) => {
                return (
                  <CommonSingleAutoComplete
                    {...field}
                    value={field.value}
                    onChange={field.onChange}
                    autoCompleteOptions={item.autoCompleteOptions}
                    error={!!errors[item.name as keyof TType]}
                    helperText={
                      errors[item.name as keyof TType]?.message as string
                    }
                    textFieldProps={{
                      ...(item.properties as TextFieldProps),
                      sx: inputSx,
                    }}
                    datum={item.values as T[]}
                  />
                );
              }}
              name={item.name as Path<TType>}
            />
          );
        case "autocomplete":
          return (
            <Controller
              key={index}
              control={control}
              render={({ field }) => {
                return (
                  <CommonAutoComplete
                    {...field}
                    value={field.value as (string | { label: string })[]}
                    onChange={field.onChange}
                    autoCompleteOptions={
                      item.autoCompleteOptions as MultipleAutoComplete<
                        string | { label: string }
                      >
                    }
                    error={!!errors[item.name as keyof TType]}
                    helperText={
                      errors[item.name as keyof TType]?.message as string
                    }
                    textFieldProps={{
                      ...(item.properties as TextFieldProps),
                      sx: inputSx,
                    }}
                    datum={item.values as (string | { label: string })[]}
                  />
                );
              }}
              name={item.name as Path<TType>}
            />
          );
        case "date":
          return (
            <CommonDateTimePicker
              key={index}
              item={item}
              control={control}
              errors={errors}
              index={index}
            />
          );
        default:
          return <Fragment></Fragment>;
      }
    },
    [control, errors, defaultValue],
  );

  return (
    <FormProvider {...methods}>
      <Fragment>
        <Box
          sx={{
            height: "100%",
            display: "flex",
            flexDirection: "column",
            position: "relative",
            overflow: "auto",
            backgroundColor: (theme) => theme.palette.background.paper,
          }}
          component="form"
          id={formName}
          onSubmit={handleSubmit(internalSubmit)}
        >
          <Stack
            spacing={1}
            sx={{
              flex: "1 1 auto",
              padding: "16px",
              ...externalSX,
            }}
          >
            {items.map((item, index) => {
              return (
                <Tooltip
                  title={item.tooltip || ""}
                  key={`${item.type}-${index}`}
                  arrow
                  placement="right"
                  disableInteractive={true}
                >
                  <span className="inline-block w-full">
                    {handleFormReturn(item, index)}
                  </span>
                </Tooltip>
              );
            })}
          </Stack>
          {children}
          <Box
            sx={{
              padding: "5px",
            }}
          >
            {hasActions && (
              <div className="w-full flex justify-end gap-3">
                {hasCancel && (
                  <Button
                    onClick={onCancel}
                    variant="contained"
                    color="secondary"
                  >
                    {secondaryButtonText || "Cancel"}
                  </Button>
                )}
                {hasSubmit && (
                  <Button
                    disabled={!isChanged || isSubmitting || isValidateFailed}
                    form={formName}
                    onClick={primaryButtonOnClick}
                    type={primaryButtonType}
                    variant="contained"
                    color="primary"
                  >
                    {isSubmitting ? "Submitting..." : "Submit"}
                  </Button>
                )}
              </div>
            )}
          </Box>
        </Box>
      </Fragment>
    </FormProvider>
  );
};
