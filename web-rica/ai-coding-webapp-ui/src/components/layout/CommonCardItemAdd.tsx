/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import AddIcon from "@mui/icons-material/Add";
import Card from "@mui/material/Card";
import CardActionArea from "@mui/material/CardActionArea";
import Typography from "@mui/material/Typography";

interface AddItemCardProps {
  onClick: (event: React.MouseEvent) => void;
  label: string;
}

export const CommonCardItemAdd = ({ onClick, label }: AddItemCardProps) => {
  return (
    <Card
      sx={(theme) => ({
        display: "flex",
        borderRadius: 1,
        border: `1px dashed ${theme.palette.divider}`,
        backgroundColor: "transparent",
      })}
    >
      <CardActionArea
        onClick={onClick}
        sx={{
          display: "flex",
          flexDirection: "column",
          justifyContent: "center",
          alignItems: "center",
          height: "100%",
          p: 3,
        }}
      >
        <AddIcon sx={{ fontSize: 40, color: "text.secondary" }} />
        <Typography sx={{ mt: 1, color: "text.secondary" }}>{label}</Typography>
      </CardActionArea>
    </Card>
  );
};
