/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import { useEffect, useState } from "react";
import {
  ButtonGroupItem,
  CommonChipModel,
  CommonChipProps,
} from "../../typings/component.props";

import Chip from "@mui/material/Chip";
import Paper from "@mui/material/Paper";
import Stack from "@mui/material/Stack";
import { CommonInputComponent } from "../../components/CommonInput";
import { promptSampleSelections, selectRules } from "./suggestion.model";
import { CommonButtonGroup } from "../../components/CommonButtonGroup";
import { useDispatch, useSelector } from "react-redux";
import {
  updateInputContext,
  updateText,
} from "../../store/reducers/input.slice";
import { RootState } from "../../store";
import { useSubmitChat } from "../../hooks/useSubmitChat";
import Grid from "@mui/material/Grid2";
import { HistorySummary } from "../chat/components/HistorySummary";
import { useTranslation } from "react-i18next";

export const ChatSuggestion = ({
  items,
  submitChat,
  ...props
}: CommonChipProps) => {
  const [selectedRules, setSelectedRules] = useState<ButtonGroupItem>(
    selectRules[0],
  );
  const { t } = useTranslation();
  const { handleSubmitChat } = useSubmitChat(submitChat);
  const history = useSelector((state: RootState) => state.dialog);
  const dispatch = useDispatch();
  const inputContext = useSelector((state: RootState) => state.input);
  const handleOnClickChip = (chipItem: CommonChipModel) => {
    dispatch(
      updateInputContext({
        inputText: (chipItem?.realText || chipItem.label) as string,
      }),
    );
  };
  useEffect(() => {
    dispatch(updateInputContext({ vector_search: [selectedRules.value] }));
  }, [selectedRules]);
  const handleChangeInputAction = async (message: string) => {
    await handleSubmitChat({ content: message });
  };

  return (
    <div className=" flex flex-col items-center w-full justify-center">
      <div className="w-full flex justify-center">
        <CommonButtonGroup
          buttonGroupItems={selectRules}
          handleSelectEachButton={function (item: ButtonGroupItem): void {
            setSelectedRules(item);
          }}
        />
      </div>
      <div className="md:w-[75%] w-full my-2 bg-ray-50 border-2 border-gray-100 bg-gray-50  rounded-xl">
        <CommonInputComponent
          value={inputContext.inputText}
          disabledSubmitButton={false}
          handleSubmit={handleChangeInputAction}
          hasOptionButton={false}
          textField={{
            rows: 4,
            variant: "standard",
            InputProps: { disableUnderline: true },
            placeholder: t("inputPlaceholder"),
          }}
          onChangeInput={function (newValue: string): void {
            dispatch(updateText(newValue));
          }}
          isMultiline={true}
        >
          {!!selectedRules.value && items.length > 0 && (
            <Stack spacing={2} {...props} className="flex w-full">
              {promptSampleSelections[selectedRules.value].map(
                ({ realText, ...chipItem }, index) => {
                  return (
                    <Grid
                      size={
                        12 / promptSampleSelections[selectedRules.value].length
                      }
                      key={index}
                    >
                      <Chip
                        onClick={() =>
                          handleOnClickChip({ ...chipItem, realText })
                        }
                        className="text-pretty !py-2 !italic"
                        sx={{ ...chipItem.sx }}
                        {...chipItem}
                      />
                    </Grid>
                  );
                },
              )}
            </Stack>
          )}
        </CommonInputComponent>
      </div>
      <Paper
        sx={{ backgroundColor: "inherit" }}
        elevation={0}
        className="w-[90%] my-4"
      >
        <h2 className="m-2">{t("recentChat")}</h2>
        <div className="flex justify-center">
          <Grid size={{ md: 12 }} container spacing={2}>
            {history.slice(0, 6).map((historyItem, index) => {
              return (
                <Grid key={index} size={{ md: 4, xs: 6 }}>
                  <HistorySummary historyItem={historyItem} />
                </Grid>
              );
            })}
          </Grid>
        </div>
      </Paper>
    </div>
  );
};
