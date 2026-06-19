/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import { END_POINT } from "../../requests/endpoint";
import { instance } from "../../requests/requests";
import { UserSettingsRequestBody } from "../../typings/request.ts";

export const getMe = async () => {
  return await instance.get(END_POINT.user);
};
export const getUserMe = async () => {
  return await instance.get(END_POINT.userMe);
};
export const createUser = async () => {
  return await instance.post(END_POINT.user);
};
export const getUserSettings = async () => {
  return await instance.get(END_POINT.userSettings);
};
export const updateUserSettings = async (
  request_body: UserSettingsRequestBody,
) => {
  return await instance.patch(END_POINT.userSettings, request_body);
};
