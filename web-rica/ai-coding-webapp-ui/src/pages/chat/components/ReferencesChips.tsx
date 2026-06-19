import React, { PropsWithChildren } from "react";
import { getFileNameFromURL } from "../../../utils/utilities";
import { GET_FILE_NAME_FROM_URL } from "../../../constants/regex";

type BaseReference = {
  id: string | number;
};

type ReferenceChipProps<T extends BaseReference> = {
  item: T;
  displayFields?: (keyof T)[];
  onRemove?: (id: T["id"]) => void;
  onClickLink?: (ev: React.MouseEvent, item: T) => void;
  linkTo?: string;
  chipColor?: "blue" | "green" | "purple" | "gray";
};

const colorStyles = {
  blue: "bg-blue-100 text-blue-800 hover:bg-blue-200",
  green: "bg-green-100 text-green-800 hover:bg-green-200",
  purple: "bg-purple-100 text-purple-800 hover:bg-purple-200",
  gray: "bg-gray-100 text-gray-800 hover:bg-gray-200",
};
export function ReferenceChip<T extends BaseReference>({
  item,
  displayFields,
  // linkTo,
  children,
  onClickLink,
  chipColor = "blue",
}: PropsWithChildren<ReferenceChipProps<T>>) {
  return (
    <div
      onClick={(ev) => onClickLink?.(ev, item)}
      className={` cursor-pointer inline-flex items-center gap-2 px-3 py-1 m-1 rounded-full text-sm transition-colors ${colorStyles[chipColor]}`}
    >
      <span className="font-medium">
        {displayFields
          ? displayFields.map((field, index) => (
              <React.Fragment key={field.toString()}>
                {index > 0 && ", "}
                {
                  getFileNameFromURL(
                    item[field] as string,
                    GET_FILE_NAME_FROM_URL
                  ) as string
                }
              </React.Fragment>
            ))
          : children}
      </span>
    </div>
  );
}
