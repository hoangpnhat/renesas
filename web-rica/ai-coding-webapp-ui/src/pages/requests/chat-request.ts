import { END_POINT } from "../../requests/endpoint";
import { instance } from "../../requests/requests";
import { BaseObject } from "../../typings";
import {
  CreateDialogRequestBody,
  MessageContentUpdate,
  SavedChatContentResponseBody,
} from "../../typings/request";

// Create methods
export async function createDialogChat(body: CreateDialogRequestBody) {
  return await instance.post(END_POINT.dialog, body);
}

// Get methods
export async function getAllDialogChat() {
  return await instance.get(END_POINT.dialogs);
}

export async function getDialogContent(id: string, params: BaseObject) {
  return await instance.get(`${END_POINT.dialog}/${id}/contents`, {
    params: params,
  });
}

// Delete method
export async function deleteDialog(id: string) {
  return await instance.delete(`${END_POINT.dialog}/${id}`);
}

export async function deleteMultipleDialogs(ids: string[]) {
  return await instance.delete(`${END_POINT.dialogs}`, { data: { ids } });
}

export async function updateDialogTitle(dialog_id: string, title: string) {
  return await instance.patch(`${END_POINT.dialog}/${dialog_id}`, {
    title: title,
  });
}

export async function getSavedChatContent(params: BaseObject) {
  return await instance.get<SavedChatContentResponseBody>(
    `${END_POINT.saved_content}`,
    { params: params },
  );
}

export async function updateMessageContent(
  dialog_id: string,
  content_id: string,
  request_body: MessageContentUpdate,
) {
  return await instance.patch(
    `${END_POINT.dialog}/${dialog_id}/content/${content_id}`,
    request_body,
  );
}
