import { useCallback, useEffect, useRef, useState } from "react";
import { AnchorContext } from "./useMenuRef";

export const useSelectionText = (
  handleAnchor: (event: React.MouseEvent<HTMLElement> | AnchorContext) => void,
  closeAnchor: () => void
) => {
  const [selectedText, setSelectedText] = useState("");
  const containerRef = useRef<any>(null);
  const handleMouseUp = useCallback((event: any) => {
    event.preventDefault();
    const selection = window.getSelection();
    if (selection) {
      const text = selection?.toString().trim();
      if (text && isSelectionWithinElement(selection, containerRef.current)) {
        setSelectedText(text);
        handleAnchor({
          mouseX: event.clientX - 20,
          mouseY: event.clientY,
        } as AnchorContext);
      } else {
        closeAnchor();
        setSelectedText("");
      }
    }
  }, []);
  const handleClickOutside = useCallback((event: any) => {
    if (containerRef.current && !containerRef.current?.contains(event.target)) {
      closeAnchor();
    }
  }, []);
  const isSelectionWithinElement = (selection: Selection, element: any) => {
    if (selection.rangeCount > 0) {
      const range = selection.getRangeAt(0);
      return element.contains(range.commonAncestorContainer);
    }
    return false;
  };
  useEffect(() => {
    document.addEventListener("mouseup", handleMouseUp);
    document.addEventListener("mousedown", handleClickOutside);
    return () => {
      document.removeEventListener("mouseup", handleMouseUp);
      document.removeEventListener("mousedown", handleClickOutside);
    };
  }, []);
  return { selectedText, containerRef };
};
