import { CommonState } from "./common.props";
import {
    CommonDialogProps,
    CommonDrawerProps,
    HistoryChatItem,
    MessageType,
} from "./component.props";
import { InputContextType } from "./context.dto";

export type MessageActionType =
    | "add"
    | "new-content"
    | "update-content"
    | "remove"
    | "update"
    | "update-status"
    | "update-all-new"
    | "update-pending"
    | "remove-pending"
    | "update-id"
    | "reset";
export type InputActionType =
    | "remove"
    | "update"
    | "update-txt"
    | "update-status";

export type DialogActionType =
    | "update"
    | "open"
    | "close"
    | "add"
    | "update-new"
    | "reset"
    | "remove";
export type DialogComponentActionType = "update" | "open" | "close";
export type DrawerComponentActionType = "update" | "open" | "close";
export type CommonActionType = "update" | "increase-page" | "decrease-page";
export interface BaseReducerAction<T, K> {
    type: T;
    newData: K;
}
export interface ChatReducerAction
    extends BaseReducerAction<
        MessageActionType,
        Partial<MessageType | MessageType[]>
    > {}

export interface InputReducerAction extends Partial<InputContextType> {
    type: InputActionType;
}

export interface DialogReducerAction
    extends BaseReducerAction<
        DialogActionType,
        Partial<HistoryChatItem> | Partial<HistoryChatItem>[]
    > {}
export interface CommonReducerAction
    extends BaseReducerAction<CommonActionType, Partial<CommonState>> {}
export interface DialogComponentReducerAction
    extends BaseReducerAction<
        DialogComponentActionType,
        Partial<CommonDialogProps>
    > {}
export interface DrawerComponentReducerAction
    extends BaseReducerAction<
        DrawerComponentActionType,
        Partial<CommonDrawerProps>
    > {}
