/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import { SelectionItemWithIcon } from "../typings/component.props.ts";
import { EnglishFlagIcon, JapanFlagIcon } from "../components/Icons.tsx";

export const languageSelection: SelectionItemWithIcon[] = [
  {
    name: "EN",
    value: "en",
    icon: <EnglishFlagIcon />,
  },
  {
    name: "JP",
    value: "ja",
    icon: <JapanFlagIcon />,
  },
];
