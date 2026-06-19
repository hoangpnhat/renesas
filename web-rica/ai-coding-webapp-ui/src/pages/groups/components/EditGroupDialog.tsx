import { useState, useEffect } from "react";
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
import { useUpdateGroupMutation } from "../../../store/api-slices/groupApiSlice";
import { Group } from "../../../typings/request";

interface EditGroupDialogProps {
  open: boolean;
  onClose: () => void;
  group: Group;
}

const EditGroupDialog = ({ open, onClose, group }: EditGroupDialogProps) => {
  const [updateGroup] = useUpdateGroupMutation();
  const [formData, setFormData] = useState({
    name: "",
    description: "",
    visibility: "",
  });

  useEffect(() => {
    if (group) {
      setFormData({
        name: group.name,
        description: group.description || "",
        visibility: group.visibility,
      });
    }
  }, [group]);

  const handleSubmit = async () => {
    try {
      await updateGroup({ id: group.id, data: formData }).unwrap();
      onClose();
    } catch (error) {
      console.error("Failed to update group:", error);
    }
  };

  return (
    <Dialog open={open} onClose={onClose} maxWidth="sm" fullWidth>
      <DialogTitle>Edit Group</DialogTitle>
      <DialogContent>
        <TextField
          fullWidth
          label="Group Name"
          value={formData.name}
          onChange={(e) => setFormData({ ...formData, name: e.target.value })}
          margin="normal"
          required
        />
        <TextField
          fullWidth
          label="Description"
          value={formData.description}
          onChange={(e) => setFormData({ ...formData, description: e.target.value })}
          margin="normal"
          multiline
          rows={3}
        />
        <FormControl fullWidth margin="normal">
          <InputLabel>Visibility</InputLabel>
          <Select
            value={formData.visibility}
            label="Visibility"
            onChange={(e) => setFormData({ ...formData, visibility: e.target.value })}
          >
            <MenuItem value="private">Private</MenuItem>
            <MenuItem value="public">Public</MenuItem>
          </Select>
        </FormControl>
      </DialogContent>
      <DialogActions>
        <Button onClick={onClose}>Cancel</Button>
        <Button onClick={handleSubmit} variant="contained" disabled={!formData.name}>
          Update
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default EditGroupDialog;
