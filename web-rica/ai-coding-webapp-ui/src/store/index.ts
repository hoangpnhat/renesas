import { configureStore } from "@reduxjs/toolkit";
import dialogReducer from "./reducers/dialog.slice";
import commonReducer from "./reducers/common.slice";
import chatReducer from "./reducers/chat.slice";
import inputReducer from "./reducers/input.slice";
import dialogComponentReducer from "./reducers/dialogComponent.slice";
import drawerReducer from "./reducers/drawer.slice";
import fileValidateStatusReducer from "./reducers/fileValidateStatus.slice";
import noteReducer from "./reducers/notes.slice";
import userReducer from "./reducers/user.slice";
import progressReducer from "./reducers/progress.slice";
import errorsReducer from "./reducers/errors.slice";
import notificationReducer from "./reducers/notification.slice";
import { modelAPISlice } from "./api-slice/modelAPISlice.ts";
import { fileApiSlice } from "./api-slices/fileApiSlice.ts";
import { knowledgeBaseAPISlice } from "./api-slices/knowledgeBaseAPISlice.ts";
import { userAPISlice } from "./api-slices/userApiSlice.ts";
import { groupApiSlice } from "./api-slices/groupApiSlice.ts";
import { notificationApiSlice } from "./api-slices/notificationApiSlice.ts";
import { customizeApiSlice } from "./api-slices/promptApiSlice.ts";

export const store = configureStore({
  devTools: import.meta.env.VITE_PRODUCTION !== "true",
  reducer: {
    dialog: dialogReducer,
    common: commonReducer,
    chat: chatReducer,
    input: inputReducer,
    dialogComponent: dialogComponentReducer,
    drawer: drawerReducer,
    progress: progressReducer,
    note: noteReducer,
    fileValidateStatus: fileValidateStatusReducer,
    user: userReducer,
    errors: errorsReducer,
    notification: notificationReducer,
    [fileApiSlice.reducerPath]: fileApiSlice.reducer,
    [userAPISlice.reducerPath]: userAPISlice.reducer,
    [knowledgeBaseAPISlice.reducerPath]: knowledgeBaseAPISlice.reducer,
    [modelAPISlice.reducerPath]: modelAPISlice.reducer,
    [groupApiSlice.reducerPath]: groupApiSlice.reducer,
    [notificationApiSlice.reducerPath]: notificationApiSlice.reducer,
    [customizeApiSlice.reducerPath]: customizeApiSlice.reducer,
  },
  middleware: (getDefaultMiddleware) =>
    getDefaultMiddleware({
      serializableCheck: false,
    }).concat([
      modelAPISlice.middleware,
      fileApiSlice.middleware,
      knowledgeBaseAPISlice.middleware,
      userAPISlice.middleware,
      groupApiSlice.middleware,
      notificationApiSlice.middleware,
      customizeApiSlice.middleware,
    ]),
});

// Infer the `RootState` and `AppDispatch` types from the store itself
export type RootState = ReturnType<typeof store.getState>;
// Inferred type: {posts: PostsState, comments: CommentsState, users: UsersState}
export type AppDispatch = typeof store.dispatch;
export const useAppDispatch: () => AppDispatch = () => store.dispatch;
