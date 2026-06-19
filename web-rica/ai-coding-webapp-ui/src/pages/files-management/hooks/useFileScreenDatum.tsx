/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */
import { useMemo } from "react";
import { MAX_FILE_SIZE } from "../../../constants/fileSize.ts";
import { BaseObject } from "../../../typings";
import { FormItem } from "../../../typings/common.props.ts";
import { User } from "../../../typings/component.props.ts";

export const useFileScreenDatum = () => {
  const formItemValue = useMemo(() => {
    const formItem: FormItem<Partial<User>, BaseObject>[] = [
      {
        type: "upload_file",
        label: "File Name",
        name: "file",
        values: "",
        isMultiple: false,
        validateFileSize: MAX_FILE_SIZE, // 20MB
      },
    ];
    return formItem;
  }, []);
  return { formItem: formItemValue };
};
