import { store } from "../store";
import { updateText } from "../store/reducers/input.slice";

export const handleActionAboutUs = <T>(item: T) => {
  store.dispatch(updateText(`🚀 ~ handleActionAboutUs ~ item: ${item}`));
};
