import React from "react";
export interface AnchorContext {
  mouseX: number;
  mouseY: number;
}
export const useMenuRef = (type: "anchorEl" | "anchorPosition") => {
  const [anchorEl, setAnchorEl] = React.useState<
    | null
    | HTMLElement
    | {
        mouseX: number;
        mouseY: number;
      }
  >(null);

  const handleClick = (
    event: React.MouseEvent<HTMLElement> | AnchorContext
  ) => {
    if (type === "anchorEl") {
      setAnchorEl((event as React.MouseEvent<HTMLElement>).currentTarget);
    } else {
      setAnchorEl(
        event as AnchorContext
        // repeated contextmenu when it is already open closes it with Chrome 84 on Ubuntu
        // Other native context menus might behave different.
        // With this behavior we prevent contextmenu from the backdrop to re-locale existing context menus.
      );
    }
  };
  const handleClose = () => {
    setAnchorEl(null);
  };
  return { handleClick, handleClose, anchorEl };
};
