import Button from "@mui/material/Button";
import Paper from "@mui/material/Paper";
import React, { useState } from "react";
import { CommonInputComponent } from "../../../components/CommonInput";
import { EditChatSectionProps } from "../../../typings/component.props";

export const EditChatSection = ({ message }: EditChatSectionProps) => {
  const [text, setText] = useState(message.content);

  const handleConfirm = (event: React.MouseEvent<HTMLElement>) => {
    event.preventDefault();
  };

  const handleCancel = (event: React.MouseEvent<HTMLElement>) => {
    event.preventDefault();
  };
  return (
    <Paper
      component={"div"}
      elevation={0}
      className="w-full shadow-box bg-gray-200"
    >
      <CommonInputComponent
        inputBase={{ multiline: true }}
        hasSubmitButton={false}
        value={text as string}
        onChangeInput={function (newValue: string): void {
          setText(newValue);
        }}
      >
        <div className="flex justify-content-between flex-row-reverse w-full">
          <Button
            className="!m-1"
            variant="contained"
            color="primary"
            onClick={handleConfirm}
          >
            Confirm
          </Button>
          <Button
            variant="contained"
            className="!m-1"
            color="secondary"
            onClick={handleCancel}
          >
            Cancel
          </Button>
        </div>
      </CommonInputComponent>
    </Paper>
  );
};
