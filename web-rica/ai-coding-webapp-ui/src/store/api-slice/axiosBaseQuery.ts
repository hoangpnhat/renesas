/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { BaseQueryFn } from "@reduxjs/toolkit/query";
import { instance } from "../../requests/requests.ts";
import { AxiosError, AxiosRequestConfig } from "axios";

export interface CustomAxiosRequest extends AxiosRequestConfig {}

// Hàm customAxiosBaseQuery sử dụng instance đã định nghĩa
const customAxiosBaseQuery: BaseQueryFn<
  CustomAxiosRequest,
  unknown,
  unknown
> = async (axiosConfig, api) => {
  try {
    const result = await instance({
      ...axiosConfig, // Spread all axios config options including paramsSerializer
      signal: api.signal,
    });
    return { data: result.data };
  } catch (error) {
    const axiosError = error as AxiosError;
    return {
      error: {
        status: axiosError.response?.status,
        data: axiosError.response?.data || axiosError.message,
      },
    };
  }
};

export default customAxiosBaseQuery;
