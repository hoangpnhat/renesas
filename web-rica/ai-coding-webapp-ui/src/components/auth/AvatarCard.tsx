/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import Avatar from "@mui/material/Avatar";
import Box from "@mui/material/Box";
import Paper from "@mui/material/Paper";
import Typography from "@mui/material/Typography";
import Badge from "@mui/material/Badge";
import { RootState } from "../../store";
import { useDispatch, useSelector } from "react-redux";
import { Fragment, useEffect, useMemo } from "react";
import { getAccountMe, getAccountPhoto } from "../../requests/common-requests";
import { updateUserDetails } from "../../store/reducers/user.slice";
import { imageToBlob } from "../../utils/utilities";
import { useGetNotificationsQuery } from "../../store/api-slices/notificationApiSlice";
import NotificationPopover from "../notifications/NotificationPopover";

import { styled } from "@mui/material/styles";
import { GraphMe } from "../../typings/request.ts";
// Styled components
const StyledPaper = styled(Paper)(({ theme }) => ({
  display: "flex",
  alignItems: "center",
  padding: theme.spacing(1.5),
  transition: "all 0.3s ease",
  cursor: "pointer",
  backgroundColor: "inherit",
  borderRadius: 0,

  // backgroundColor: "rgb(243, 244, 246)", // Matches Tailwind's bg-gray-100
  "&:hover": {
    backgroundColor: theme.palette.action.hover,
    boxShadow: theme.shadows[4],
  },
}));
export const StyledAvatar = styled(Avatar)(({ theme }) => ({
  width: 42,
  height: 42,
  border: `2px solid ${theme.palette.primary.main}`,
  boxShadow: theme.shadows[2],
}));

const UserInfo = styled(Box)(({ theme }) => ({
  marginLeft: theme.spacing(2),
  flex: 1,
}));

const UserName = styled(Typography)(({ theme }) => ({
  fontWeight: 600,
  color: theme.palette.text.primary,
  fontSize: "1.1rem",
  lineHeight: 1.2,
}));

const UserRole = styled(Typography)(({ theme }) => ({
  color: theme.palette.text.secondary,
  fontSize: "0.9rem",
  fontStyle: "italic",
  margin: 2,
}));
const AvatarCard = () => {
  const { avatarUrl, department, name } = useSelector(
    (state: RootState) => state.user,
  );
  const dispatch = useDispatch();

  const { data: notificationsData } = useGetNotificationsQuery({
    page: 0,
    entries: 20,
  });

  const unreadCount = useMemo(() => {
    if (!notificationsData?.data) return 0;
    return notificationsData.data.filter((n) => n.status === "unread").length;
  }, [notificationsData]);

  // Debug: log notifications data and unread count to verify values at runtime
  useEffect(() => {}, [notificationsData, unreadCount]);

  useEffect(() => {
    const getUserInfo = async () => {
      const userDetail: GraphMe = (await getAccountMe()).data;

      const userPhoto = await getAccountPhoto();
      dispatch(
        updateUserDetails({
          avatarUrl: imageToBlob(userPhoto.data),
          department: userDetail.jobTitle,
          name: userDetail.displayName,
          email: userDetail.mail,
        }),
      );
    };
    getUserInfo();
  }, [dispatch]);

  return (
    <Fragment>
      <StyledPaper elevation={0}>
        {/* Notification center anchored to the avatar */}
        <NotificationPopover
          anchorElement={
            <Badge
              badgeContent={unreadCount}
              color="error"
              overlap="circular"
              anchorOrigin={{
                vertical: "top",
                horizontal: "right",
              }}
            >
              <StyledAvatar src={avatarUrl} alt={name} />
            </Badge>
          }
        />
        <UserInfo>
          <UserName variant="h6">{name}</UserName>
          <UserRole variant="subtitle2">{department}</UserRole>
        </UserInfo>
      </StyledPaper>
    </Fragment>
  );
};

export default AvatarCard;
