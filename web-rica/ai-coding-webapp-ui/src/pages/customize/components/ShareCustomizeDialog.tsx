import { useEffect, useState } from "react";
import Dialog from "@mui/material/Dialog";
import DialogTitle from "@mui/material/DialogTitle";
import DialogContent from "@mui/material/DialogContent";
import DialogActions from "@mui/material/DialogActions";
import Button from "@mui/material/Button";
import TextField from "@mui/material/TextField";
import List from "@mui/material/List";
import ListItem from "@mui/material/ListItem";
import ListItemButton from "@mui/material/ListItemButton";
import ListItemText from "@mui/material/ListItemText";
import Select from "@mui/material/Select";
import MenuItem from "@mui/material/MenuItem";
import FormControl from "@mui/material/FormControl";
import InputLabel from "@mui/material/InputLabel";
import Box from "@mui/material/Box";
import Tabs from "@mui/material/Tabs";
import Tab from "@mui/material/Tab";
import IconButton from "@mui/material/IconButton";
import DeleteIcon from "@mui/icons-material/Delete";
import Divider from "@mui/material/Divider";
import Typography from "@mui/material/Typography";
import CircularProgress from "@mui/material/CircularProgress";
import { useGetUsersQuery } from "../../../store/api-slices/userApiSlice";
import { useSearchGroupQuery } from "../../../store/api-slices/groupApiSlice";
import { ShareCustomizeRequest } from "../../../store/api-slices/promptApiSlice";
import { SharedResource } from "../../../store/api-slices/knowledgeBaseAPISlice";
import { User } from "../../../typings/component.props";
import { Group } from "../../../typings/request";
import { useTranslation } from "react-i18next";

interface ShareCustomizeDialogProps {
  open: boolean;
  onClose: () => void;
  title: string;
  shares: SharedResource[];
  isLoadingShares: boolean;
  isSharing: boolean;
  onShare: (requestBody: ShareCustomizeRequest[]) => Promise<void>;
}

interface ShareItem extends ShareCustomizeRequest {
  name: string;
}

const ShareCustomizeDialog = ({
  open,
  onClose,
  title,
  shares,
  isLoadingShares,
  isSharing,
  onShare,
}: ShareCustomizeDialogProps) => {
  const { t } = useTranslation();
  const [tabValue, setTabValue] = useState(0);
  const [searchQuery, setSearchQuery] = useState("");
  const [selectedTarget, setSelectedTarget] = useState<{
    id: string;
    name: string;
    type: "user" | "group";
  } | null>(null);
  const [role, setRole] = useState<"viewer" | "developer" | "owner">("viewer");
  const [shareList, setShareList] = useState<ShareItem[]>([]);

  const { data: usersData, isLoading: isLoadingUsers } = useGetUsersQuery(
    { query: searchQuery, limit: 10 },
    { skip: searchQuery.length < 2 || tabValue !== 0 },
  );

  const { data: groupsData, isLoading: isLoadingGroups } = useSearchGroupQuery(
    {
      page: 0,
      limit: 10,
      search_text: searchQuery,
      sort_field: "last_modified",
    },
    { skip: searchQuery.length < 2 || tabValue !== 1 },
  );

  useEffect(() => {
    if (open) {
      setShareList(
        shares.map((share) => ({
          target_id: share.resource_id,
          target_type: share.resource_type,
          role: share.role || "viewer",
          name: share.resource_name,
        })),
      );
    }
  }, [shares, open]);

  const handleTabChange = (_: React.SyntheticEvent, v: number) => {
    setTabValue(v);
    setSearchQuery("");
    setSelectedTarget(null);
  };

  const handleSelectUser = (user: User) => {
    setSelectedTarget({
      id: user.id,
      name: user.full_name || user.email,
      type: "user",
    });
    setSearchQuery("");
  };

  const handleSelectGroup = (group: Group) => {
    setSelectedTarget({ id: group.id, name: group.name, type: "group" });
    setSearchQuery("");
  };

  const handleAddShare = () => {
    if (!selectedTarget) return;
    const exists = shareList.some((s) => s.target_id === selectedTarget.id);
    if (exists) {
      setShareList((prev) =>
        prev.map((s) =>
          s.target_id === selectedTarget.id ? { ...s, role } : s,
        ),
      );
    } else {
      setShareList((prev) => [
        ...prev,
        {
          target_id: selectedTarget.id,
          target_type: selectedTarget.type,
          role,
          name: selectedTarget.name,
        },
      ]);
    }
    setSelectedTarget(null);
    setRole("viewer");
    setSearchQuery("");
  };

  const handleRemoveShare = (targetId: string) => {
    setShareList((prev) => prev.filter((s) => s.target_id !== targetId));
  };

  const handleSubmit = async () => {
    try {
      await onShare(
        shareList.map(({ target_id, target_type, role }) => ({
          target_id,
          target_type,
          role,
        })),
      );
      handleClose();
    } catch (e) {
      console.error("Failed to share:", e);
    }
  };

  const handleClose = () => {
    setSearchQuery("");
    setSelectedTarget(null);
    setTabValue(0);
    setShareList([]);
    onClose();
  };

  return (
    <Dialog open={open} onClose={handleClose} maxWidth="md" fullWidth>
      <DialogTitle>
        {t("customize.shareTitle")} — <em>{title}</em>
      </DialogTitle>
      <DialogContent>
        {isLoadingShares ? (
          <Box sx={{ display: "flex", justifyContent: "center", py: 4 }}>
            <CircularProgress />
          </Box>
        ) : (
          <>
            <Box sx={{ mb: 3 }}>
              <Typography variant="subtitle1" sx={{ mb: 1, fontWeight: 600 }}>
                {t("Current Shares")} ({shareList.length})
              </Typography>
              {shareList.length === 0 ? (
                <Typography variant="body2" color="text.secondary">
                  {t("customize.noShares")}
                </Typography>
              ) : (
                <List
                  dense
                  sx={{ border: 1, borderColor: "divider", borderRadius: 1 }}
                >
                  {shareList.map((share, i) => (
                    <Box key={share.target_id}>
                      {i > 0 && <Divider />}
                      <ListItem
                        secondaryAction={
                          <IconButton
                            edge="end"
                            onClick={() => handleRemoveShare(share.target_id)}
                          >
                            <DeleteIcon />
                          </IconButton>
                        }
                      >
                        <ListItemText
                          primary={share.name}
                          secondary={`${share.target_type} • ${share.role}`}
                        />
                      </ListItem>
                    </Box>
                  ))}
                </List>
              )}
            </Box>

            <Divider sx={{ my: 2 }} />

            <Typography variant="subtitle1" sx={{ mb: 2, fontWeight: 600 }}>
              {t("Add New Share")}
            </Typography>

            <Tabs value={tabValue} onChange={handleTabChange} sx={{ mb: 2 }}>
              <Tab label={t("Share to Member")} />
              <Tab label={t("Share to Group")} />
            </Tabs>

            <TextField
              fullWidth
              label={tabValue === 0 ? t("Search Users") : t("Search Groups")}
              value={searchQuery}
              onChange={(e) => setSearchQuery(e.target.value)}
              margin="normal"
              placeholder={
                tabValue === 0
                  ? t("Type at least 2 characters to search users...")
                  : t("Type at least 2 characters to search groups...")
              }
              helperText={
                searchQuery.length > 0 && searchQuery.length < 2
                  ? t("Please enter at least 2 characters")
                  : ""
              }
            />

            {searchQuery.length >= 2 && (
              <Box sx={{ mb: 2, maxHeight: 200, overflow: "auto" }}>
                <List dense>
                  {(tabValue === 0 ? isLoadingUsers : isLoadingGroups) ? (
                    <ListItem>
                      <ListItemText primary={t("Loading...")} />
                    </ListItem>
                  ) : tabValue === 0 ? (
                    usersData?.data.length === 0 ? (
                      <ListItem>
                        <ListItemText primary={t("No users found")} />
                      </ListItem>
                    ) : (
                      usersData?.data.map((user) => (
                        <ListItem key={user.id} disablePadding>
                          <ListItemButton
                            onClick={() => handleSelectUser(user)}
                            selected={selectedTarget?.id === user.id}
                          >
                            <ListItemText
                              primary={user.full_name || user.id}
                              secondary={user.email}
                            />
                          </ListItemButton>
                        </ListItem>
                      ))
                    )
                  ) : groupsData?.data.length === 0 ? (
                    <ListItem>
                      <ListItemText primary={t("No groups found")} />
                    </ListItem>
                  ) : (
                    groupsData?.data.map((group) => (
                      <ListItem key={group.id} disablePadding>
                        <ListItemButton
                          onClick={() => handleSelectGroup(group)}
                          selected={selectedTarget?.id === group.id}
                        >
                          <ListItemText
                            primary={group.name}
                            secondary={group.description}
                          />
                        </ListItemButton>
                      </ListItem>
                    ))
                  )}
                </List>
              </Box>
            )}

            {selectedTarget && (
              <Box sx={{ mt: 2 }}>
                <Box
                  sx={{
                    mb: 2,
                    p: 2,
                    bgcolor: "primary.50",
                    border: 2,
                    borderColor: "primary.main",
                    borderRadius: 1,
                  }}
                >
                  <Typography variant="body2" sx={{ mb: 2, fontWeight: 600 }}>
                    <strong>{t("customize.selected")}:</strong>{" "}
                    {selectedTarget.name} ({selectedTarget.type})
                  </Typography>
                  <FormControl fullWidth sx={{ mb: 2 }}>
                    <InputLabel>{t("Role")}</InputLabel>
                    <Select
                      value={role}
                      label={t("Role")}
                      onChange={(e) =>
                        setRole(
                          e.target.value as "viewer" | "developer" | "owner",
                        )
                      }
                    >
                      <MenuItem value="viewer">{t("Viewer")}</MenuItem>
                      <MenuItem value="developer">{t("Developer")}</MenuItem>
                      <MenuItem value="owner">{t("Owner")}</MenuItem>
                    </Select>
                  </FormControl>
                  <Button
                    onClick={handleAddShare}
                    variant="contained"
                    fullWidth
                  >
                    {t("Add to Share List")}
                  </Button>
                </Box>
              </Box>
            )}
          </>
        )}
      </DialogContent>
      <DialogActions>
        <Button onClick={handleClose}>{t("labelButton.cancel")}</Button>
        <Button onClick={handleSubmit} variant="contained" disabled={isSharing}>
          {isSharing ? t("Saving...") : t("Save Shares")}
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default ShareCustomizeDialog;
