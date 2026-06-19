/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import Card from "@mui/material/Card";
import CardContent from "@mui/material/CardContent";
import Tooltip from "@mui/material/Tooltip";
import Typography from "@mui/material/Typography";
import { HistorySummaryProps } from "../../../typings/component.props";
import { getCalendarNameDay } from "../../../utils/dayjs";
import { useNavigate } from "react-router-dom";
import ForumOutlinedIcon from "@mui/icons-material/ForumOutlined";
import { useTranslation } from "react-i18next";
import { Language } from "../../../typings/common.props.ts";

export const HistorySummary = ({ historyItem }: HistorySummaryProps) => {
  const navigate = useNavigate();
  const handleCardActionClick = (ev: React.MouseEvent) => {
    ev.preventDefault();
    navigate(`/chat/${historyItem.id}`);
  };
  const { t, i18n } = useTranslation();
  return (
    <Tooltip
      title={
        <span>
          Navigate to <strong>{historyItem.title}</strong>
        </span>
      }
      placement="top-end"
      arrow
    >
      <Card
        sx={{ height: 180, backgroundColor: "inherit" }}
        variant="outlined"
        className="relative !rounded-2xl cursor-pointer hover:bg-[#D3D3D3] transition-all duration-300 ease-in-out hover:transform hover:translate-y-1"
        onClick={handleCardActionClick}
      >
        <div className="w-full relative h-[10%] my-2">
          <ForumOutlinedIcon className="absolute left-[5px] top-[5px]" />
        </div>
        <CardContent className="flex flex-col space-y-4">
          <Typography sx={{ fontWeight: 700 }}>{historyItem.title}</Typography>
          <Typography
            sx={{
              position: "absolute",
              bottom: 0,
              color: "text.secondary",
              mb: 1.5,
            }}
          >
            {t("text.lastModified")}:{" "}
            {getCalendarNameDay(
              historyItem.datetime as string,
              i18n.language as Language,
            )}
          </Typography>
        </CardContent>
      </Card>
    </Tooltip>
  );
};
