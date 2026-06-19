import FormGroup from "@mui/material/FormGroup";
import FormControlLabel from "@mui/material/FormControlLabel";
import Checkbox from "@mui/material/Checkbox";
import { CheckBoxGroupProps } from "../typings/component.props.ts";
import { ChangeEvent } from "react";

import Tooltip from "@mui/material/Tooltip";
import classNames from "classnames";

export interface CommonInterface {
  [K: string]: boolean;
}

export const CheckboxLabels = ({
  checkBoxItem,
  onChangeLabel,
  checkState,
}: CheckBoxGroupProps) => {
  const handleChange = (event: ChangeEvent<HTMLInputElement>) => {
    onChangeLabel({
      ...checkState,
      [event.target.name]: event.target.checked,
    });
  };

  return (
    <FormGroup sx={{ flexDirection: "row" }}>
      {checkBoxItem &&
        checkBoxItem.map((item) => (
          <Tooltip
            title={`${checkState[item.value] ? "Disable" : "Enable"} RAG for ${item.name}`}
            arrow={true}
            key={`${item.value}`}
            placement="top"
          >
            <FormControlLabel
              className={classNames({
                "text-gray-400": !checkState[item.value],
              })}
              control={
                <Checkbox
                  checked={checkState[item.value]}
                  onChange={handleChange}
                  name={item.value}
                />
              }
              label={item.name}
            />
          </Tooltip>
        ))}
    </FormGroup>
  );
};
