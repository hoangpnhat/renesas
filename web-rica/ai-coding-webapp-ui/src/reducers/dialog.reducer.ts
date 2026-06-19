import { HistoryChatItem } from "../typings/component.props";
import { DialogReducerAction } from "../typings/reducer";

export function dialogReducer(
    dialog: Partial<HistoryChatItem>[],
    action: DialogReducerAction
) {
    const { type } = action;
    switch (type) {
        case "remove":
            return dialog.filter(
                (content) =>
                    content.id !== (action.newData as HistoryChatItem).id
            );
        case "close":
            return dialog;
        case "open":
            return dialog;
        case "update-new":
            return action.newData as HistoryChatItem[];
        case "add":
            return Array.isArray(action.newData)
                ? [...dialog, ...action.newData]
                : [...dialog, action.newData];
        case "reset":
            return [];
        default:
            return dialog;
    }
}
