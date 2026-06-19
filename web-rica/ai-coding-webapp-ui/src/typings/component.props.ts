/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import {
  BaseSelectProps,
  Breakpoint,
  CardProps,
  ChipProps,
  InputBaseProps,
  MenuProps,
  StackProps,
  TextFieldProps
} from "@mui/material";
import { PropsWithChildren } from "react";
import { BaseActionMenu, BaseInterface } from ".";
import { Message, RetrievedDocs } from "./request";
import { AppBarProps as MuiAppBarProps } from "@mui/material/AppBar";
import { UserSelectionState } from "../pages/chat/typings/state";
import { CommonInterface } from "../components/CommonFormGroup.tsx";
import { CommonDialogActionsProps } from "../components/dialog/CommonDialogActions.tsx";
import { CommonSize } from "./common.props.ts";

export interface SidebarProps {
  anchor: Anchor;
  isOpen: boolean;
}

export interface BaseElement {}

export interface CommonDrawerProps extends CommonDialogProps {
  drawerWidth: number | string;
  anchor: Anchor;
  maxDrawerWidth: number | string;
  keepMounted?: boolean;
  position?: "absolute" | "relative" | "unset";
  isOverflow?: boolean;
}

export interface CommonInputProps {
  inputBase?: InputBaseProps;
  headers?: JSX.Element;
  isMultiline?: boolean;
  hasOptionButton?: boolean;
  handleSubmit?: (value: any) => void;
  onChangeInput?: (newValue: string) => void;
  value?: string;
  hasSubmitButton?: boolean;
  disabledSubmitButton?: boolean;
  textField?: TextFieldProps;
  optionButton?: JSX.Element;
}

export interface CommonRadioGroupProps {
  label: string;
  selectionItems: RadioGroupSelectItem[];
}

export type RadioGroupSelectItem = SelectionItem & {
  control?: JSX.Element;
  isDisabled?: boolean;
};

export interface CommonTableProps<T> {
  // [key: string]: string;
  listColumnNames: string[];
  itemsRows: T[];
  displayColumns: BaseObjectString;
}

export interface BaseObjectString {
  [key: string]: string;
}

export interface BoxChatSectionProps
  extends Omit<CommonInputProps, "inputBase" | "hasOptionButton"> {
  boxes: Partial<MessageType>[];
  handleSubmit: (message: Message) => void;
}

export interface UserSelectionProps {
  selectionArrayConfigs: SelectionOptionConfigs[];
  onChangeSelectOption?: (item: UserSelectionState) => void;
  title: string;
}

export interface ChatBoxSectionProps {
  message: Partial<MessageType>;
  cardProps?: CardProps;
  menuItems?: MenuItemType[];
  toolbarArr?: ChatToolBarItem[];
  order: number;
}

export interface MessageType {
  content: string;
  role: Role;
  id: string;
  status: MessageStatus;
  has_error: boolean;
  retrieved_docs?: RetrievedDocs[];
  is_favorite: boolean;
  vector_search: string;
  categories: string[];
  isEmphasized?: boolean;
  is_good_response?: 1 | -1 | 0;
}

export interface File extends BaseInterface {
  name: string;
  file_url: string;
  file_id: string;
  file_name: string;
  added_at: string;
  owner_name: string;
}

export interface CheckBoxGroupProps {
  checkBoxItem: SelectionItem[];
  onChangeLabel: (value: CommonInterface) => void;
  checkState: CommonInterface;
}

export interface CommonExpansionProps {
  title: string;
  unMountOnExit: boolean;
  extraClassName?: string;
}

export interface CommonMenuProps<T extends BaseInterface | string>
  extends MenuProps {
  menuItems: MenuItemType[];
  handleClose?: () => void;
  selectedItem: T;
  actionRef: BaseActionMenu<string | HistoryChatItem>;
  contextMenuPosition: {
    mouseX: number;
    mouseY: number;
  } | null;
  type: "anchorEl" | "anchorPosition";
}

export interface MenuItemType {
  text: string;
  key: string;
  icon: JSX.Element;
  shortcutText?: string;
  handleAction?: <T extends BaseInterface>(model: T) => void;
}

export type CommonTextFieldProps = TextFieldProps;

export interface CommonDialogProps
  extends PropsWithChildren,
    CommonDialogActionsProps {
  isOpen: boolean;
  title: string;
  onClose?: () => void;
  hasCloseButton?: boolean;
  onSubmit?: () => void;
  confirmButtonText: string;
  cancelButtonText: string;
  hasCancelButton: boolean;
  hasAction?: boolean;
  maxWidth?: Breakpoint;
  fullWidth?: boolean;
  typeSubmit?: "submit" | "button" | "reset";
  size?: Partial<CommonSize>;
  helpText?: string;
  componentStacks?: OmitPartialCommonDialogProps[];

  isDisabledConfirmButton?: boolean;
}

export interface TypingTextProps {
  role?: Role;
  text?: string;
  ref?: HTMLElement;
}

export interface CommonLoadingProps {
  isLoading: boolean;
}

export interface CommonChipProps extends StackProps {
  items: ChipProps[];
  submitChat: (message: Message) => void;
}

export interface NewChatProps extends Omit<CommonChipProps, "items"> {}

export interface ReactMarkdownProps {
  message: MessageType;
  hasTyping?: boolean;
}

export interface HistoryChatItem extends BaseElement, BaseInterface {
  title: string;
  datetime: string | Date;
  hasTyping?: boolean;
}

export interface AppBarProps extends MuiAppBarProps, LayoutProps {}

export interface CommonSidebarProps extends LayoutProps {
  sidebarItems: SideBarItem[];

  isOpen: boolean;
}

export type ApplicationRole = "member" | "owner";

export interface Owner extends BaseInterface {
  full_name: string;
}

export interface Member extends Owner {
  role: ApplicationRole;
  added_at: string;
}

export interface User extends Member {
  user_id: string;
  email: string;
}

export interface SideBarItem {
  to: string;
  name: string;
  key: string;
  icon: JSX.Element;
  disabled: boolean;
  roles?: string[];
  toolNames?: string[];
  children?: SideBarItem[];
  toggleKey?: string;
  disabledHelpText?: string;
  isActive?: boolean;
}

export interface LayoutProps {
  drawerWidth: number;
}

export interface HistoryChatProps {
  history: HistoryChatItem[];
  hasSearch?: boolean;
}

export interface Reference {
  content: string;
  doc_uri: string;
  name: string;
  file_name: string;
  id: string;
}

export interface Status {
  state: string;
  message: string;
}

export interface ChatToolBarItem {
  title: string;
  icon: JSX.Element;
  outlineItem?: JSX.Element;
  onClickHandle?: (event: React.MouseEvent, item: MessageType) => void;
  name: string;
}

export interface ReferenceSectionProps {
  referencesItem: RetrievedDocs[];
}

export interface SelectionOptionConfigs {
  selectionArr: SelectionItem[];
  titleString: string;
  keyString: string;
  isMultiple: boolean;
}

export interface ChatToolBarProps {
  listToolbar: ChatToolBarItem[];
  messageItem: MessageType;
  item: MessageType;
  extraClassName?: string;
}

export interface CommonSelectionProps
  extends BaseSelectProps,
    SelectionOptionConfigs {
  onChangeSelection: (
    value: SelectionItem[] | SelectionItem | null,
    key: string,
  ) => void;
  isDisable: boolean;
  selectedValue: SelectionItem | SelectionItem[];
}

export interface TitleContentBarProps
  extends Omit<UserSelectionProps, "selectionArrayConfigs" | "title"> {
  selectedItem: HistoryChatItem;
}

export interface SelectionItem {
  name: string;
  value: string;
}

export interface SelectionItemWithIcon extends SelectionItem {
  icon: JSX.Element;
}

export interface HistoryChatTransform {
  [key: string]: Partial<HistoryChatTransformItem>;
}

export interface HistoryChatTransformItem extends HistoryChatProps {
  value: number;
  isCollapse: boolean;
}

export interface HistorySummaryProps {
  historyItem: HistoryChatItem;
}

export interface CollapseComponentProps {
  open: boolean;
}

export interface ReferenceDetailsProps {
  refItem: RetrievedDocs;
}

export interface CommonButtonGroupProps {
  buttonGroupItems: ButtonGroupItem[];
  handleSelectEachButton: (item: ButtonGroupItem) => void;
}

export interface ButtonGroupItem extends SelectionItem {}

export interface UserDefaultSetting {
  knowledge_base_id: string;
  tool: string;
}

export interface AvatarCardProps {
  name: string;
  department: string;
  email: string;
  roles: string[];
  avatarUrl: string;
}

export interface LanguageSwitcherProps {
  selectionItems: SelectionItemWithIcon[];
}

export type EditChatSectionProps = Omit<
  ChatBoxSectionProps,
  "cardProps" | "menuItems"
>;

export interface OmitPartialCommonDialogProps
  extends Omit<Partial<CommonDialogProps>, "isOpen"> {}

export type CommonChipModel = ChipProps & { realText?: string };
export type Role = "user" | "bot" | "assistant";
export type Anchor = "left" | "right" | "top" | "bottom";
export type MessageStatus =
  | "fulfill"
  | "completed"
  | "pending"
  | "queuing"
  | "";
export type StatusSnackbar = "success" | "warning" | "error" | "info";

export interface BaseQueryParamsV2 {
  page?: number;
  limit?: number;
  sort_field?: string;
  sort_direction?: string;
}