/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import React, { PropsWithChildren } from "react";
import { ReferenceSectionProps } from "../../../typings/component.props";
import "./chat.scss";
import { RetrievedDocs } from "../../../typings/request";
import { ReferenceChip } from "./ReferencesChips";
import { useTranslation } from "react-i18next";
import SourceOutlinedIcon from "@mui/icons-material/SourceOutlined";
import { useDispatch } from "react-redux";
import { updateDrawerConfig } from "../../../store/reducers/drawer.slice.ts";
import { ReferenceDetails } from "./ReferencesDetails.tsx";

export const ReferenceSection = ({
  referencesItem,
}: PropsWithChildren<ReferenceSectionProps>) => {
  const dispatch = useDispatch();
  const handleOnClickLink = (ev: React.MouseEvent, item: RetrievedDocs) => {
    ev.preventDefault();
    dispatch(
      updateDrawerConfig({
        isOpen: true,
        children: <ReferenceDetails refItem={item} />,
        title: "Details",
        drawerWidth: "40%",
        maxDrawerWidth: "40%",
        anchor: "right",
        // position: "absolute",
        keepMounted: false,
        isOverflow: false,
      }),
    );
  };
  const { t } = useTranslation();
  return (
    <React.Fragment>
      <span className="flex ">
        <SourceOutlinedIcon sx={{ marginRight: 1 }} />
        <h2 className="font-bold">{t("title.references")}:</h2>
      </span>

      {referencesItem?.map((refItem, index) => {
        return (
          <ReferenceChip
            key={index}
            onClickLink={handleOnClickLink}
            item={refItem}
            displayFields={["doc_uri"]}
          />
        );
      })}
    </React.Fragment>
  );
};
