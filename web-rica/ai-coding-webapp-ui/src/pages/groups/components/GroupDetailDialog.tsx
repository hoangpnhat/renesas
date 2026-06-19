import Dialog from "@mui/material/Dialog";
import DialogTitle from "@mui/material/DialogTitle";
import DialogContent from "@mui/material/DialogContent";
import DialogActions from "@mui/material/DialogActions";
import Button from "@mui/material/Button";
import Typography from "@mui/material/Typography";
import Box from "@mui/material/Box";
import Chip from "@mui/material/Chip";
import List from "@mui/material/List";
import ListItem from "@mui/material/ListItem";
import ListItemText from "@mui/material/ListItemText";
import { Group } from "../../../typings/request";
import {
  useDeclineGroupInvitationMutation,
  useJoinGroupMutation,
} from "../../../store/api-slices/groupApiSlice";

interface GroupDetailDialogProps {
  open: boolean;
  onClose: () => void;
  group: Group;
  onUpdate: () => void;
}

const GroupDetailDialog = ({
  open,
  onClose,
  group,
  onUpdate,
}: GroupDetailDialogProps) => {
  const [joinGroup] = useJoinGroupMutation();
  const [declineGroupInvitation] = useDeclineGroupInvitationMutation();
  const handleJoin = async () => {
    try {
      await joinGroup(group.id);
      onUpdate();
      onClose();
    } catch (error) {
      console.error("Failed to join group:", error);
    }
  };

  const handleDecline = async () => {
    try {
      await declineGroupInvitation(group.id);
      onUpdate();
      onClose();
    } catch (error) {
      console.error("Failed to decline invitation:", error);
    }
  };

  return (
    <Dialog open={open} onClose={onClose} maxWidth="md" fullWidth>
      <DialogTitle>{group.name}</DialogTitle>
      <DialogContent>
        <Box sx={{ mb: 2 }}>
          <Typography variant="body1" gutterBottom>
            {group.description || "No description"}
          </Typography>
          <Box sx={{ display: "flex", gap: 1, mt: 2 }}>
            <Chip label={group.visibility} />
            <Chip label={`Owner: ${group.owner_name || "Unknown"}`} />
          </Box>
        </Box>

        <Typography variant="h6" sx={{ mt: 3, mb: 2 }}>
          Members ({group.members.length})
        </Typography>
        <List>
          {group.members.map((member) => (
            <ListItem key={member.user_id}>
              <ListItemText
                primary={member.full_name || member.user_id}
                secondary={member.role}
              />
            </ListItem>
          ))}
        </List>
      </DialogContent>
      <DialogActions>
        <Button onClick={onClose}>Close</Button>
        <Button onClick={handleDecline} color="error">
          Decline
        </Button>
        <Button onClick={handleJoin} variant="contained">
          Join
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default GroupDetailDialog;
