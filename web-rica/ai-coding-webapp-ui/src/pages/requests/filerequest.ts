/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { instance } from "../../requests/requests.ts";
import { END_POINT } from "../../requests/endpoint.ts";

export const getFileDownload = async (id: string) => {
  return instance.get(`/${END_POINT.file}/${id}/download`, {
    headers: { skipLoading: true },
    responseType: "blob",
  });
};
