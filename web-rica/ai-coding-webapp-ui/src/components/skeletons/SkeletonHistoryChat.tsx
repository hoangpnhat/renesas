/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import Box from "@mui/material/Box";
import Skeleton from "@mui/material/Skeleton";
import Typography from "@mui/material/Typography";

export const SkeletonHistoryChat = () => (
  <Box sx={{ width: "100%", bgcolor: "grey.100" }}>
    <Typography variant="h6" component="div" sx={{ p: 2, bgcolor: "grey.100" }}>
      <Skeleton width="100%" />
    </Typography>
    {[...Array(10)].map((_, index) => (
      <Box key={index} sx={{ p: 2 }}>
        <Skeleton width="80%" />
      </Box>
    ))}
  </Box>
);
