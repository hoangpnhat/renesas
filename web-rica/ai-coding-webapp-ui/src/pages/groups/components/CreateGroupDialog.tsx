import { useState } from "react";
import Dialog from "@mui/material/Dialog";
import DialogTitle from "@mui/material/DialogTitle";
import DialogContent from "@mui/material/DialogContent";
import DialogActions from "@mui/material/DialogActions";
import Button from "@mui/material/Button";
import TextField from "@mui/material/TextField";
import Select from "@mui/material/Select";
import MenuItem from "@mui/material/MenuItem";
import FormControl from "@mui/material/FormControl";
import InputLabel from "@mui/material/InputLabel";
import List from "@mui/material/List";
import ListItem from "@mui/material/ListItem";
import ListItemText from "@mui/material/ListItemText";
import ListItemSecondaryAction from "@mui/material/ListItemSecondaryAction";
import IconButton from "@mui/material/IconButton";
import AddIcon from "@mui/icons-material/Add";
import RemoveIcon from "@mui/icons-material/Remove";
import Box from "@mui/material/Box";
import { useCreateGroupMutation } from "../../../store/api-slices/groupApiSlice";
import { useGetUsersQuery } from "../../../store/api-slices/userApiSlice";
import { CreateGroupRequest } from "../../../typings/request";
import { User } from "../../../typings/component.props";
import { useTranslation } from "react-i18next";

interface CreateGroupDialogProps {
  open: boolean;
  onClose: () => void;
}

const CreateGroupDialog = ({ open, onClose }: CreateGroupDialogProps) => {
  const [createGroup] = useCreateGroupMutation();
  const [searchQuery, setSearchQuery] = useState("");
  const [formData, setFormData] = useState<CreateGroupRequest>({
    name: "",
    description: "",
    visibility: "private",
    members: [],
  });
  const { t } = useTranslation();

  const { data: usersData, isLoading } = useGetUsersQuery(
    { query: searchQuery, limit: 10 },
    { skip: searchQuery.length < 2 },
  );

  const handleAddMember = (user: User) => {
    if (!formData.members.find((m) => m.user_id === user.id)) {
      setFormData({
        ...formData,
        members: [
          ...formData.members,
          { user_id: user.id, role: "member", full_name: user.full_name },
        ],
      });
    }
  };

  const handleRemoveMember = (userId: string) => {
    setFormData({
      ...formData,
      members: formData.members.filter((m) => m.user_id !== userId),
    });
  };

  const handleSubmit = async () => {
    try {
      await createGroup(formData).unwrap();
      setFormData({
        name: "",
        description: "",
        visibility: "private",
        members: [],
      });
      setSearchQuery("");
      onClose();
    } catch (error) {
      console.error("Failed to create group:", error);
    }
  };

  return (
    <Dialog open={open} onClose={onClose} maxWidth="sm" fullWidth>
      <DialogTitle>{t("Create New Group")}</DialogTitle>
      <DialogContent>
        <TextField
          fullWidth
          label={t("Group Name")}
          value={formData.name}
          onChange={(e) => setFormData({ ...formData, name: e.target.value })}
          margin="normal"
          required
        />
        <TextField
          fullWidth
          label={t("Description")}
          value={formData.description}
          onChange={(e) =>
            setFormData({ ...formData, description: e.target.value })
          }
          margin="normal"
          multiline
          rows={3}
        />
        <FormControl fullWidth margin="normal">
          <InputLabel>{t("Visibility")}</InputLabel>
          <Select
            value={formData.visibility}
            label={t("Visibility")}
            onChange={(e) =>
              setFormData({ ...formData, visibility: e.target.value })
            }
          >
            <MenuItem value="private">{t("Private")}</MenuItem>
            <MenuItem value="public">{t("Public")}</MenuItem>
          </Select>
        </FormControl>

        <TextField
          fullWidth
          label={t("Search Users to Invite")}
          value={searchQuery}
          onChange={(e) => setSearchQuery(e.target.value)}
          margin="normal"
          placeholder={t("Type to search users...")}
        />

        {searchQuery.length >= 2 && (
          <Box sx={{ mb: 2 }}>
            <List dense>
              {isLoading ? (
                <ListItem>
                  <ListItemText primary={t("Loading...")} />
                </ListItem>
              ) : usersData?.data.length === 0 ? (
                <ListItem>
                  <ListItemText primary={t("No users found")} />
                </ListItem>
              ) : (
                usersData?.data.map((user) => (
                  <ListItem key={user.id}>
                    <ListItemText
                      primary={user.full_name || user.id}
                      secondary={user.email}
                    />
                    <ListItemSecondaryAction>
                      <IconButton
                        edge="end"
                        onClick={() => handleAddMember(user)}
                      >
                        <AddIcon />
                      </IconButton>
                    </ListItemSecondaryAction>
                  </ListItem>
                ))
              )}
            </List>
          </Box>
        )}

        {formData.members.length > 0 && (
          <>
            <Box sx={{ mt: 2, mb: 1 }}>
              <strong>Members to Invite ({formData.members.length})</strong>
            </Box>
            <List dense>
              {formData.members.map((member) => (
                <ListItem key={member.user_id}>
                  <ListItemText primary={member.full_name} />
                  <ListItemSecondaryAction>
                    <IconButton
                      edge="end"
                      onClick={() => handleRemoveMember(member.user_id)}
                    >
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
        <Button onClick={onClose}>{t("Cancel")}</Button>
        <Button
          onClick={handleSubmit}
          variant="contained"
          disabled={!formData.name}
        >
          {t("Create")}
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default CreateGroupDialog;
