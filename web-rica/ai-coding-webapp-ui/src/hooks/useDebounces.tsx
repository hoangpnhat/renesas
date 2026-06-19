/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { useCallback, useState } from "react";
import debounce from "lodash/debounce";

export const useDebounce = <T = string,>(initValue: T, delay: number) => {
  const [value, setValue] = useState(initValue);
  const debouncedSetValue = useCallback(
    debounce((newValue: T) => setValue(newValue), delay),
    [setValue, value],
  );
  return { value, debouncedSetValue };
};
