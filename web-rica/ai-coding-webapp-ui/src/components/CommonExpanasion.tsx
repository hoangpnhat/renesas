import Accordion from "@mui/material/Accordion";
import AccordionDetails from "@mui/material/AccordionDetails";
import AccordionSummary from "@mui/material/AccordionSummary";
import Typography from "@mui/material/Typography";
import React, { PropsWithChildren } from "react";
import ExpandMoreIcon from "@mui/icons-material/ExpandMore";
import { CommonExpansionProps } from "../typings/component.props";

export const CommonExpansion = ({
  children,
  title,
  unMountOnExit,
  extraClassName,
}: PropsWithChildren<CommonExpansionProps>) => {
  return (
    <React.Fragment>
      <Accordion
        className={extraClassName as string}
        sx={{ boxShadow: "unset" }}
        slotProps={{
          transition: { unmountOnExit: unMountOnExit },
        }}
      >
        <AccordionSummary
          expandIcon={<ExpandMoreIcon />}
          aria-controls="panel1-content"
          id="panel1-header"
        >
          <Typography sx={{ fontWeight: "bold" }} variant="subtitle2">
            {title}
          </Typography>
        </AccordionSummary>
        <AccordionDetails>
          {children || <Typography>There is nothing here</Typography>}
        </AccordionDetails>
      </Accordion>
    </React.Fragment>
  );
};
