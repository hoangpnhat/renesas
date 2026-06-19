import { CommonDrawerProps } from "../typings/component.props";
import { DrawerComponentReducerAction } from "../typings/reducer";

export function commonDrawerReducer(
    drawer: Partial<CommonDrawerProps>,
    action: DrawerComponentReducerAction
) {
    const { type, newData } = action;
    switch (type) {
        case "update":
            return { ...drawer, ...newData };
        case "close":
            return { ...drawer, isOpen: false };
        case "open":
            return { ...drawer, isOpen: true };
        default:
            return drawer;
    }
}
