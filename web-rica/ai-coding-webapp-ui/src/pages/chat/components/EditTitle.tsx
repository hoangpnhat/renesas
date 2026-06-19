/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import Button from "@mui/material/Button";
import TextField from "@mui/material/TextField";
import { ChangeEvent, Fragment, useEffect, useRef, useState } from "react";
import { useDispatch } from "react-redux";

import { closeDialog } from "../../../store/reducers/dialogComponent.slice";
import { useTranslation } from "react-i18next";

interface EditTitleProps {
  onHandleSubmit: (value: string) => void;
  valueDefault: string;
}

const EditTitle = ({ onHandleSubmit, valueDefault }: EditTitleProps) => {
  const [value, setValue] = useState(valueDefault);
  const dispatch = useDispatch();
  const onChangeHandleInput = (
    event: ChangeEvent<HTMLInputElement | HTMLTextAreaElement>,
  ) => {
    setValue(event.target.value);
  };
  const inputRef = useRef<HTMLInputElement>();

  useEffect(() => {
    if (inputRef.current) {
      // Move the cursor to the end after the component mounts
      inputRef.current.setSelectionRange(
        inputRef.current.value.length,
        inputRef.current.value.length,
      );
    }
  }, []);
  const { t } = useTranslation();
  const handleClose = (event: React.MouseEvent) => {
    event.preventDefault();
    dispatch(closeDialog());
  };
  const handleSubmitClick = (event: React.MouseEvent, value: string) => {
    handleClose(event);
    onHandleSubmit(value);
  };
  return (
    <Fragment>
      <TextField
        inputRef={inputRef}
        autoFocus={true}
        size="small"
        multiline
        value={value}
        onChange={onChangeHandleInput}
        id="outlined-basic"
        className="w-full"
        variant="outlined"
      />
      <div className="w-full flex justify-end space-x-2 py-2">
        <Button onClick={handleClose} variant="contained" color="secondary">
          {t("labelButton.cancel")}
        </Button>
        <Button
          onClick={(event) => handleSubmitClick(event, value)}
          variant="contained"
          color="primary"
        >
          {t("labelButton.confirm")}
        </Button>
      </div>
    </Fragment>
  );
};

export default EditTitle;
