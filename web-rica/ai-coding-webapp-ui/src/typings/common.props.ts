/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import { Theme as SystemTheme } from "@mui/system/createTheme/createTheme";
import type { SxProps } from "@mui/system/styleFunctionSx/styleFunctionSx";
import type { SelectItem } from "../components/form/CommonSelections";

import type { InputProps } from "@mui/material/Input/Input";
import type { ButtonProps } from "@mui/material/Button/Button";
import type { SelectProps } from "@mui/material/Select/Select";
import type { TextFieldProps } from "@mui/material/TextField/TextField";
import { ChangeEvent, MouseEvent, ReactNode } from "react";
import type { AutocompleteProps } from "@mui/material/Autocomplete";
import { BaseObjectWithInterface } from "./index";
import type { StatusSnackbar } from "./component.props";

import type { DatePickerProps } from "@mui/x-date-pickers";

export interface CommonState {
  isLoading: boolean;
  snackBar: Partial<SnackbarConfig>;
  isSelectedSidebar: string;
  isThinkingChatBot: boolean;
  pageBreak: Partial<PageBreak>;
  language: Language;
  isShowingSideBar: boolean;
  toggleKey?: string;
}

export interface ExternalSx<Theme extends object = SystemTheme> {
  externalSX?: SxProps<Theme>;
}

export interface ExternalSx<Theme extends object = SystemTheme> {
  externalSX?: SxProps<Theme>;
}

export interface ExternalSx<Theme extends object = SystemTheme> {
  externalSX?: SxProps<Theme>;
}

export interface SnackbarConfig {
  isOpen: boolean;
  message: string;
  type: StatusSnackbar;
}

export interface PageBreak {
  page: number;
  total_records: number;
  total_page: number;
}
export type Language = "en" | "ja";

export interface CommonSize {
  height: number | string;
  width: number | string;
  maxHeight: number | string;
  maxWidth: number | string;
  minHeight: number | string;
  minWidth: number | string;
}

type FormType =
  | "selection"
  | "input"
  | "upload_file"
  | "single-autocomplete"
  | "autocomplete"
  | "date"
  | "text_display"
  | "checkbox"
  | "max_chars_input"
  | "run-api";
type FormValueType<T> = SelectItem[] | string | T[];
export type PropertiesType<T> =
  | InputProps
  | ButtonProps
  | TextFieldProps
  | SelectProps
  | DatePickerProps
  | MultipleAutoComplete<T>;

export interface FilterParams {
  page?: number;
  entries?: number;
  query?: string | null;
  value_name?: string;
}

export type ButtonType = "text" | "icon-button";

export interface CommonActionsProps<T> {
  text: string;
  actionHandle: (event: MouseEvent<HTMLButtonElement>, items?: T) => void;
  type?: ButtonType;
  icon?: ReactNode;
  isDisabled: boolean;
  disabledFunc?: (item: T) => boolean;
}

export type MultipleAutoComplete<T> =
  | (Partial<AutocompleteProps<T, true, true, false>> & { multiple: true })
  | (Partial<AutocompleteProps<T, false, true, false>> & { multiple?: false });

export interface CommonSize {
  height: number | string;
  width: number | string;
  maxHeight: number | string;
  maxWidth: number | string;
  minHeight: number | string;
  minWidth: number | string;
}

export type AdditionalFormItemProps = {
  maxChars?: number;
};

export interface FormItem<
  T,
  K extends BaseObjectWithInterface<unknown> = BaseObjectWithInterface<unknown>,
> {
  type: FormType;
  label: string;
  name: string;
  tooltip?: string;
  values?: FormValueType<T>;
  autoCompleteOptions?: MultipleAutoComplete<T>;
  properties?: (PropertiesType<T> | undefined) & AdditionalFormItemProps;
  onChangeHandle?: (value: string) => void;
  customAcceptString?: string | ((value: K) => string);
  additionalErrorMessage?: string;
  isMultiple?: boolean;
  validateFileSize?: number; // in bytes
}

export interface PaginationAction {
  page: number;
  entries: number;
  total: number;
  handleChangePage: (
    event: MouseEvent<HTMLButtonElement> | null,
    newPage: number,
  ) => void;
  handleChangeRowsPerPage: (
    event: ChangeEvent<HTMLInputElement | HTMLTextAreaElement>,
  ) => void;
}

export interface PaginationProps {
  pagination: PaginationAction;
}

export interface CommonActionsProps<T> {
  text: string;
  actionHandle: (event: MouseEvent<HTMLButtonElement>, items?: T) => void;
  type?: ButtonType;
  icon?: ReactNode;
  isDisabled: boolean;
  disabledFunc?: (item: T) => boolean;
}
