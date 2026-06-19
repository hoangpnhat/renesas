/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import { AxiosResponse } from "axios";
import {
  removeDialogItem,
  updateDialogWithNewItem,
} from "../../../store/reducers/dialog.slice";
import {
  DeleteDialogResponseBody,
  UpdatedDialogResponseBody,
} from "../../../typings/request";
import { deleteDialog, updateDialogTitle } from "../../requests/chat-request";
import { useNavigate, useParams } from "react-router-dom";
import { BaseActionMenu } from "../../../typings";
import { useMemo } from "react";
import { updateDialogConfig } from "../../../store/reducers/dialogComponent.slice";
import { useDispatch } from "react-redux";
import EditTitle from "../components/EditTitle";
import { HistoryChatItem } from "../../../typings/component.props";
import { useTranslation } from "react-i18next";

export const useDialogActions = () => {
  const dispatch = useDispatch();
  const { dialog_id } = useParams();
  const navigate = useNavigate();
  const { t } = useTranslation();
  const actionRef: BaseActionMenu<HistoryChatItem | string> = useMemo(() => {
    return {
      rename: (model) => {
        dispatch(
          updateDialogConfig({
            isOpen: true,
            hasAction: false,
            title: "title.renameDialog",
            fullWidth: false,
            children: (
              <EditTitle
                valueDefault={(model as HistoryChatItem).title}
                onHandleSubmit={async (value: string) => {
                  const responseBody: AxiosResponse<UpdatedDialogResponseBody> =
                    await updateDialogTitle(
                      (model as HistoryChatItem).id,
                      value,
                    );
                  dispatch(
                    updateDialogWithNewItem({
                      title: responseBody.data.results.title,
                      id: responseBody.data.results.id,
                      datetime: responseBody.data.results.last_modified,
                    }),
                  );
                }}
              />
            ),
          }),
        );
      },
      delete: async (model) => {
        dispatch(
          updateDialogConfig({
            isOpen: true,
            title: t("title.deleteDialog"),
            fullWidth: false,
            hasConfirmButton: true,
            hasCancelButton: true,
            hasAction: true,
            children: <span>{t("text.confirmDeleteDialog")}</span>,
            onSubmit: async () => {
              const responseBody: AxiosResponse<DeleteDialogResponseBody> =
                await deleteDialog((model as HistoryChatItem).id);
              dispatch(removeDialogItem({ id: responseBody.data.id }));
              if ((model as HistoryChatItem).id === dialog_id) {
                navigate("/chat/undefined");
              }
            },
          }),
        );
      },
    };
  }, [dialog_id]);
  return { actionRef };
};
