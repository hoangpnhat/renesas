import { BaseQueryFn } from "@reduxjs/toolkit/query";
import { AxiosError, AxiosRequestConfig } from "axios";
import { instance } from "../../requests/requests.ts";

export interface CustomAxiosRequest extends AxiosRequestConfig {
  urlVersion?: string;
}

export interface BaseQueryErrorProps {
  status: number;
  data: unknown | string;
  payload?: string | null | unknown;
}

// Hàm customAxiosBaseQuery sử dụng instance đã định nghĩa
const customAxiosBaseQuery: BaseQueryFn<
  CustomAxiosRequest,
  unknown,
  BaseQueryErrorProps
> = async ({ urlVersion = 'v1', url, method, data, params, headers, onUploadProgress }, api) => {
  try {
    const result = await instance({
      url: `${urlVersion}/${url}`, // added versioning to the API endpoint URL
      method,
      data,
      params,
      headers,
      onUploadProgress,
      signal: api.signal,
    });
    // Trả về dữ liệu theo chuẩn của RTK Query
    return { data: result.data };
  } catch (error) {
    const axiosError = error as AxiosError;

    let payload: null | string = null;
    if (data instanceof FormData) {
      // get file in FormData as payload for further action
      if (data.get("file")) {
        payload = (data.get("file") as File).name;
      }
    }

    return {
      error: {
        status: axiosError.response?.status || 408, // common error or timeout exception
        data: axiosError.response?.data || axiosError.message,
        payload: payload,
      },
    };
  }
};

export default customAxiosBaseQuery;
