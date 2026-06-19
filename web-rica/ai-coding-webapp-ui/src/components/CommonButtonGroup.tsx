/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import Button from "@mui/material/Button";
import ButtonGroup from "@mui/material/ButtonGroup";

import {
  ButtonGroupItem,
  CommonButtonGroupProps,
} from "../typings/component.props";
import { useState } from "react";
import { useTranslation } from "react-i18next";

export const CommonButtonGroup = ({
  buttonGroupItems,
  handleSelectEachButton,
}: CommonButtonGroupProps) => {
  const [selectedItem, setSelectedItem] = useState(buttonGroupItems[0].value);
  const { t } = useTranslation();
  const onButtonHandle = (event: React.MouseEvent, item: ButtonGroupItem) => {
    event.preventDefault();
    setSelectedItem(item.value);
    handleSelectEachButton(item);
  };
  return (
    <ButtonGroup variant="contained" aria-label="Basic button group">
      {buttonGroupItems.map((buttonItem) => {
        return (
          <Button
            key={buttonItem.value}
            color={selectedItem === buttonItem.value ? "primary" : "inherit"}
            onClick={(event) => onButtonHandle(event, buttonItem)}
          >
            {t(`${buttonItem.name}`)}
          </Button>
        );
      })}
    </ButtonGroup>
  );
};
