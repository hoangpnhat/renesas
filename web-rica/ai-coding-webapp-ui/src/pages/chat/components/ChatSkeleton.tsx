import Box from "@mui/material/Box/Box";
import Paper from "@mui/material/Paper/Paper";
import Skeleton from "@mui/material/Skeleton/Skeleton";

export const ChatSkeleton = () => {
  return (
    <Box
      sx={{
        width: "100%",
        height: "100%",
        margin: "auto",
        padding: 2,
        backgroundColor: "inherit",
        display: "flex",
        flexDirection: "column-reverse",
      }}
    >
      <Paper
        sx={{ backgroundColor: "inherit" }}
        className="flex  w-full flex-col-reverse"
        elevation={0}
      >
        <div className="mx-1 flex w-full">
          <Skeleton variant="circular" width={48} height={48} />
          <div className="max-w-full w-full flex my-3 group px-2 !rounded-2xl relative flex-col">
            <Skeleton variant="rectangular" width="100%" height={90}></Skeleton>
          </div>
        </div>
        <div className="mx-1 flex w-full">
          {/* <Skeleton variant="circular" width={48} height={48} /> */}
          <div className="max-w-full w-full flex my-3 group px-2 !rounded-2xl relative flex-row-reverse">
            <Skeleton variant="rectangular" width="100%" height={90}></Skeleton>
          </div>
        </div>
        <div className="mx-1 flex w-full">
          <Skeleton variant="circular" width={48} height={48} />
          <div className="max-w-full w-full flex my-3 group px-2 !rounded-2xl relative flex-col">
            <Skeleton variant="rectangular" width="100%" height={90}></Skeleton>
          </div>
        </div>
        <div className="mx-1 flex w-full">
          {/* <Skeleton variant="circular" width={48} height={48} /> */}
          <div className="max-w-full w-full flex my-3 group px-2 !rounded-2xl relative flex-row-reverse">
            <Skeleton variant="rectangular" width="100%" height={90}></Skeleton>
          </div>
        </div>
        <div className="mx-1 flex w-full">
          <Skeleton variant="circular" width={48} height={48} />
          <div className="max-w-full w-full flex my-3 group px-2 !rounded-2xl relative flex-col">
            <Skeleton variant="rectangular" width="100%" height={90}></Skeleton>
          </div>
        </div>
        <div className="mx-1 flex w-full">
          {/* <Skeleton variant="circular" width={48} height={48} /> */}
          <div className="max-w-full w-full flex my-3 group px-2 !rounded-2xl relative flex-row-reverse">
            <Skeleton variant="rectangular" width="100%" height={90}></Skeleton>
          </div>
        </div>
      </Paper>
    </Box>
  );
};
