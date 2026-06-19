import { useState } from "react";
import Dialog from "@mui/material/Dialog";
import DialogTitle from "@mui/material/DialogTitle";
import DialogContent from "@mui/material/DialogContent";
import DialogActions from "@mui/material/DialogActions";
import Button from "@mui/material/Button";
import TextField from "@mui/material/TextField";
import List from "@mui/material/List";
import ListItem from "@mui/material/ListItem";
import ListItemText from "@mui/material/ListItemText";
import ListItemSecondaryAction from "@mui/material/ListItemSecondaryAction";
import IconButton from "@mui/material/IconButton";
import AddIcon from "@mui/icons-material/Add";
import RemoveIcon from "@mui/icons-material/Remove";
import Select from "@mui/material/Select";
import MenuItem from "@mui/material/MenuItem";
import FormControl from "@mui/material/FormControl";
import InputLabel from "@mui/material/InputLabel";
import Box from "@mui/material/Box";
import { useGetUsersQuery } from "../../../store/api-slices/userApiSlice";
import { useUpdateMembersMutation } from "../../../store/api-slices/groupApiSlice";
import { User } from "../../../typings/component.props";
import { useEffect } from "react";

interface InviteMembersDialogProps {
  open: boolean;
  onClose: () => void;
  groupId: string;
  currentMembers?: { user_id: string; role: string; full_name: string }[];
}

const InviteMembersDialog = ({ open, onClose, groupId, currentMembers = [] }: InviteMembersDialogProps) => {
  const [searchQuery, setSearchQuery] = useState("");
  const [selectedMembers, setSelectedMembers] = useState<{ user_id: string; role: string; full_name: string }[]>([]);
  const [updateMembers] = useUpdateMembersMutation();

  useEffect(() => {
    if (open && currentMembers.length > 0) {
      setSelectedMembers(currentMembers);
    }
  }, [open, currentMembers]);

  const { data: usersData, isLoading } = useGetUsersQuery(
    { query: searchQuery, limit: 10 },
    { skip: searchQuery.length < 2 }
  );

  const handleAddMember = (user: User) => {
    if (!selectedMembers.find(m => m.user_id === user.id)) {
      setSelectedMembers([...selectedMembers, {
        user_id: user.id,
        role: "member",
        full_name: user.full_name || user.email
      }]);
    }
  };

  const handleRemoveMember = (userId: string) => {
    setSelectedMembers(selectedMembers.filter(m => m.user_id !== userId));
  };

  const handleRoleChange = (userId: string, role: string) => {
    setSelectedMembers(selectedMembers.map(m =>
      m.user_id === userId ? { ...m, role } : m
    ));
  };

  const handleSubmit = async () => {
    try {
      await updateMembers({
        groupId,
        members: selectedMembers.map(m => ({ user_id: m.user_id, role: m.role }))
      }).unwrap();
      setSelectedMembers([]);
      setSearchQuery("");
      onClose();
    } catch (error) {
      console.error("Failed to update members:", error);
    }
  };

  return (
    <Dialog open={open} onClose={onClose} maxWidth="sm" fullWidth>
      <DialogTitle>Manage Members</DialogTitle>
      <DialogContent>
        <TextField
          fullWidth
          label="Search Users"
          value={searchQuery}
          onChange={(e) => setSearchQuery(e.target.value)}
          margin="normal"
          placeholder="Type to search users..."
        />

        {searchQuery.length >= 2 && (
          <Box sx={{ mb: 2 }}>
            <List dense>
              {isLoading ? (
                <ListItem>
                  <ListItemText primary="Loading..." />
                </ListItem>
              ) : usersData?.data.length === 0 ? (
                <ListItem>
                  <ListItemText primary="No users found" />
                </ListItem>
              ) : (
                usersData?.data.map((user) => (
                  <ListItem key={user.id}>
                    <ListItemText primary={user.full_name || user.email} secondary={user.id} />
                    <ListItemSecondaryAction>
                      <IconButton edge="end" onClick={() => handleAddMember(user)}>
                        <AddIcon />
                      </IconButton>
                    </ListItemSecondaryAction>
                  </ListItem>
                ))
              )}
            </List>
          </Box>
        )}

        {selectedMembers.length > 0 && (
          <>
            <Box sx={{ mt: 2, mb: 1 }}>
              <strong>Selected Members ({selectedMembers.length})</strong>
            </Box>
            <List>
              {selectedMembers.map((member) => (
                <ListItem key={member.user_id}>
                  <ListItemText primary={member.full_name} />
                  <FormControl size="small" sx={{ minWidth: 120, mr: 1 }}>
                    <InputLabel>Role</InputLabel>
                    <Select
                      value={member.role}
                      label="Role"
                      onChange={(e) => handleRoleChange(member.user_id, e.target.value)}
                    >
                      <MenuItem value="member">Member</MenuItem>
                      <MenuItem value="admin">Admin</MenuItem>
                    </Select>
                  </FormControl>
                  <ListItemSecondaryAction>
                    <IconButton edge="end" onClick={() => handleRemoveMember(member.user_id)}>
                      <RemoveIcon />
                    </IconButton>
                  </ListItemSecondaryAction>
                </ListItem>
              ))}
            </List>
          </>
        )}
      </DialogContent>
      <DialogActions>
        <Button onClick={onClose}>Cancel</Button>
        <Button
          onClick={handleSubmit}
          variant="contained"
        >
          Save
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default InviteMembersDialog;
