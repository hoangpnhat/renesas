/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import { Fragment } from "react/jsx-runtime";
import { ChatSuggestion } from "../../suggestions/Suggestions";
import { getTimeOfDay } from "../../../utils/dayjs";
import LightModeIcon from "@mui/icons-material/LightMode";
import ModeNightIcon from "@mui/icons-material/ModeNight";
import { useSocketAction } from "../../../hooks/useSocketAction";
import { environment } from "../../../environments/environment.dev";

import { useTranslation } from "react-i18next";
import { useMemo } from "react";
import RICALogo from "/image/RICA_icon_medium.png";

const NewChatSection = () => {
  const { handleSubmit } = useSocketAction();
  const { t } = useTranslation();
  const currentDay = useMemo(() => {
    return getTimeOfDay();
  }, []);
  return (
    <Fragment>
      <div className="h-full w-full flex flex-col items-center mt-2 overflow-auto">
        <div className="w-full h-full flex justify-start items-center flex-col relative">
          <img
            alt="RICA logo"
            className="absolute top-0 left-0 mx-2"
            src={RICALogo}
          />
          <h2 className="font-bold text-3xl font">
            <span className="mr-2">
              {["morning", "afternoon"].includes(currentDay) ? (
                <LightModeIcon />
              ) : (
                <ModeNightIcon />
              )}
            </span>
            {t(`dayTime.${currentDay}`)},{" "}
            {t("text.welcomeTo", { projectName: environment.projectName })}
            <br />
            <span className="text-center flex w-full justify-center">
              ({environment.preferredProjectName})
            </span>
          </h2>
          <div className="my-5 w-full flex-col flex justify-between">
            <ChatSuggestion
              submitChat={handleSubmit}
              items={[]}
              direction="row"
            />
          </div>
        </div>
      </div>
    </Fragment>
  );
};
export default NewChatSection;
