import { CommonState } from "../typings/common.props";
import { CommonReducerAction } from "../typings/reducer";

export function commonReducer(
    commonType: Partial<CommonState>,
    action: CommonReducerAction,
) {
    const { type, newData } = action;
    switch (type) {
        case "update":
            return {
                ...commonType,
                ...newData,
            };
        case "decrease-page":
            return {
                ...commonType,
                pageBreak: {
                    ...commonType.pageBreak,
                    page: (commonType.pageBreak?.page as number) - 1,
                },
            };
        case "increase-page":
            return {
                ...commonType,
                pageBreak: {
                    ...commonType.pageBreak,
                    page: (commonType.pageBreak?.page as number) + 1,
                },
            };
        default:
            return commonType;
    }
}
