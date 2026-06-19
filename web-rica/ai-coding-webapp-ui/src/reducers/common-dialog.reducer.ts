import { CommonDialogProps } from "../typings/component.props";
import { DialogComponentReducerAction } from "../typings/reducer";

export function dialogComponentReducer(
    dialog: Partial<CommonDialogProps>,
    action: DialogComponentReducerAction,
) {
    const { type, newData } = action;
    switch (type) {
        case "close":
            return { ...dialog, isOpen: false };
        case "open":
            return { ...dialog, isOpen: true };
        case "update":
            return { ...dialog, ...newData };
        default:
            return dialog;
    }
}
