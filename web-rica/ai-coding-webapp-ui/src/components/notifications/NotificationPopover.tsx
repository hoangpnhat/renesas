import React, { useState } from "react";
import Popover from "@mui/material/Popover";
import List from "@mui/material/List";
import ListItem from "@mui/material/ListItem";
import ListItemText from "@mui/material/ListItemText";
import Typography from "@mui/material/Typography";
import Box from "@mui/material/Box";
import Button from "@mui/material/Button";
import IconButton from "@mui/material/IconButton";
import Badge from "@mui/material/Badge";
import Dialog from "@mui/material/Dialog";
import DialogTitle from "@mui/material/DialogTitle";
import DialogContent from "@mui/material/DialogContent";
import DialogContentText from "@mui/material/DialogContentText";
import DialogActions from "@mui/material/DialogActions";
import NotificationsIcon from "@mui/icons-material/Notifications";
import {
  useGetNotificationsQuery,
  useMarkNotificationAsReadMutation,
} from "../../store/api-slices/notificationApiSlice";
import {
  useJoinGroupMutation,
  useDeclineGroupInvitationMutation,
} from "../../store/api-slices/groupApiSlice";

interface Props {
  anchorElement?: React.ReactElement;
}

const NotificationPopover = ({ anchorElement }: Props) => {
  const [anchorEl, setAnchorEl] = useState<HTMLElement | null>(null);
  const { data: notificationsData } = useGetNotificationsQuery({
    page: 0,
    entries: 20,
  });
  const [markAsRead] = useMarkNotificationAsReadMutation();
  const [joinGroup] = useJoinGroupMutation();
  const [declineGroup] = useDeclineGroupInvitationMutation();

  const unreadCount =
    notificationsData?.data.filter((n) => n.status === "unread").length || 0;

  const handleClick = (event: React.MouseEvent<HTMLElement>) => {
    setAnchorEl(event.currentTarget as HTMLElement);
  };

  const handleClose = () => {
    setAnchorEl(null);
  };

  const [selectedNotification, setSelectedNotification] = useState<any | null>(
    null,
  );
  const [dialogOpen, setDialogOpen] = useState(false);

  const openDialogFor = (notification: any) => {
    setSelectedNotification(notification);
    setDialogOpen(true);
  };

  const closeDialog = () => {
    setSelectedNotification(null);
    setDialogOpen(false);
  };

  const handleAction = async (
    notificationId: string,
    action: string,
    resourceId: string,
  ) => {
    try {
      if (action === "join") {
        await joinGroup(resourceId).unwrap();
      } else if (action === "decline") {
        await declineGroup(resourceId).unwrap();
      }
      await markAsRead(notificationId).unwrap();
    } catch (error) {
      console.error("Failed to perform action:", error);
    } finally {
      closeDialog();
    }
  };

  const open = Boolean(anchorEl);

  const renderAnchor = () => {
    if (anchorElement) {
      return React.cloneElement(anchorElement, { onClick: handleClick });
    }
    return (
      <IconButton onClick={handleClick} color="inherit">
        <Badge badgeContent={unreadCount} color="error">
          <NotificationsIcon />
        </Badge>
      </IconButton>
    );
  };

  return (
    <>
      {renderAnchor()}
      <Popover
        open={open}
        anchorEl={anchorEl}
        onClose={handleClose}
        anchorOrigin={{
          vertical: "bottom",
          horizontal: "right",
        }}
        transformOrigin={{
          vertical: "top",
          horizontal: "right",
        }}
      >
        <Box sx={{ width: 360, maxHeight: 400 }}>
          <Box sx={{ p: 2, borderBottom: 1, borderColor: "divider" }}>
            <Typography variant="h6">Notifications</Typography>
          </Box>
          <List sx={{ p: 0 }}>
            {notificationsData?.data.length === 0 ? (
              <ListItem>
                <ListItemText primary="No notifications" />
              </ListItem>
            ) : (
              notificationsData?.data.map((notification) => (
                <ListItem
                  key={notification.id}
                  onClick={() => {
                    // Mark notification as read when clicked
                    markAsRead(notification.id)
                      .unwrap()
                      .then(() => {
                        if (notification.notification_type === "kb_shared") {
                          window.open(
                            `/management/knowledgeBase?page=0&entries=10&query=${notification.metadata?.kb_name}`,
                          );
                        }
                      })
                      .catch((e) => console.error("markAsRead failed", e));
                  }}
                  sx={{
                    bgcolor:
                      notification.status === "unread"
                        ? "action.hover"
                        : "inherit",
                    flexDirection: "column",
                    alignItems: "flex-start",
                    borderBottom: 1,
                    borderColor: "divider",
                    cursor: "pointer",
                  }}
                >
                  <ListItemText
                    primary={notification.title}
                    secondary={notification.message}
                  />
                  {notification.notification_type === "group_invitation" && (
                    <Box sx={{ display: "flex", gap: 1, mt: 1 }}>
                      {notification.metadata?.action_completed === true ? (
                        <Typography
                          variant="caption"
                          color="text.secondary"
                          sx={{ fontStyle: "italic" }}
                        >
                          Expired
                        </Typography>
                      ) : (
                        <Button
                          size="small"
                          variant="outlined"
                          onClick={(e) => {
                            e.stopPropagation();
                            openDialogFor(notification);
                          }}
                        >
                          Respond
                        </Button>
                      )}
                    </Box>
                  )}
                </ListItem>
              ))
            )}
          </List>
        </Box>
      </Popover>

      <Dialog open={dialogOpen} onClose={closeDialog}>
        <DialogTitle>{selectedNotification?.title || "Invitation"}</DialogTitle>
        <DialogContent>
          <DialogContentText>{selectedNotification?.message}</DialogContentText>
          {selectedNotification?.metadata && (
            <Box sx={{ mt: 2 }}>
              <Typography variant="subtitle2">Details</Typography>
              <Typography variant="body2">
                Group: {selectedNotification?.metadata.group_name}
              </Typography>
              <Typography variant="body2">
                Inviter: {selectedNotification?.metadata.inviter_name}
              </Typography>
            </Box>
          )}
        </DialogContent>
        <DialogActions>
          {selectedNotification?.metadata?.action_completed === true ? (
            <Typography
              variant="body2"
              color="text.secondary"
              sx={{ fontStyle: "italic", px: 2 }}
            >
              This invitation has expired
            </Typography>
          ) : (
            <>
              <Button
                onClick={() =>
                  handleAction(
                    selectedNotification.id,
                    "decline",
                    selectedNotification.resource_id,
                  )
                }
                color="error"
              >
                Decline
              </Button>
              <Button
                onClick={() =>
                  handleAction(
                    selectedNotification.id,
                    "join",
                    selectedNotification.resource_id,
                  )
                }
                variant="contained"
              >
                Accept
              </Button>
            </>
          )}
        </DialogActions>
      </Dialog>
    </>
  );
};

export default NotificationPopover;
