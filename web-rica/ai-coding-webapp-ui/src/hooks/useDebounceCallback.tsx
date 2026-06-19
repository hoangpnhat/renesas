/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { useCallback, useRef } from "react";

/**
 * A custom hook for creating a debounced function.
 *
 * @template TArgs - Array of argument types
 * @template TResult - Return type of the callback function
 * @param {(...args: TArgs) => TResult} callback - The function to debounce
 * @param {number} delay - The delay in milliseconds
 * @returns {(...args: TArgs) => void} - The debounced function
 */
function useDebouncedCallback<TArgs extends unknown[], TResult>(
  callback: (...args: TArgs) => TResult,
  delay = 500,
): (...args: TArgs) => void {
  const timerRef = useRef<NodeJS.Timeout | null>(null);

  return useCallback(
    (...args: TArgs) => {
      // Clear any existing timeout
      if (timerRef.current) {
        clearTimeout(timerRef.current);
      }

      // Set a new timeout
      timerRef.current = setTimeout(() => {
        callback(...args);
      }, delay);
    },
    [callback, delay],
  );
}

export default useDebouncedCallback;
