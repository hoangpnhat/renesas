/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import axios, { AxiosError } from "axios";
import { environment } from "../environments/environment.dev";
import { store } from "../store";
import { updateCommon } from "../store/reducers/common.slice";
import { acquireToken } from "../msal/auth";
import { HTTP_METHOD } from "../constants/common";

export const instance = axios.create({
  baseURL: environment.baseURL,
  headers: {
    "Content-Type": "application/json",
    "Access-Control-Allow-Origin": "*",
    "Access-Control-Allow-Methods": "*",
    "Access-Control-Allow-Headers": "*",
    "Ocp-Apim-Subscription-Key": environment.apimAccessKey,
  },
});
instance.interceptors.request.use(
  async function (config) {
    let scopes: string[] | undefined;
    if (config.headers.get("scopes")) {
      scopes = config.headers.get("scopes") as string[];
    }
    config.headers.Authorization = `Bearer ${await acquireToken(scopes)}`;
    if (!config.headers.get("skipLoading")) {
      store.dispatch(updateCommon({ isLoading: true }));
    }
    return config;
  },
  function () {
    store.dispatch(
      updateCommon({
        isLoading: false,
        snackBar: {
          isOpen: true,
          message: "Something went wrong!",
          type: "error",
        },
      }),
    );
  },
);

instance.interceptors.response.use(
  function (response) {
    if (response.config.method !== HTTP_METHOD.GET) {
      store.dispatch(
        updateCommon({
          snackBar: {
            isOpen: true,
            message: response.data?.status?.message || "Successfully",
            type: "success",
          },
        }),
      );
    }
    if (!response.config.headers["skipLoading"]) {
      store.dispatch(updateCommon({ isLoading: false }));
    }

    return response;
  },
  async function (_err: AxiosError) {
    if (!_err.config?.headers?.["skipLoading"]) {
      store.dispatch(
        updateCommon({
          isLoading: false,
          snackBar: {
            isOpen: true,
            message: "Something went wrong!",
            type: "error",
          },
        }),
      );
    }
    // if(_err.code)
    // if (_err.status === 401) {
    //   return await logoutApplication();
    // }
    return _err;
  },
);
