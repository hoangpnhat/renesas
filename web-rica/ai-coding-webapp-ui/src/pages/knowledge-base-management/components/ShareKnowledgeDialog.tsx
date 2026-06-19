/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { useState, useEffect } from "react";
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
import {
  useShareKnowledgeMutation,
  useGetKnowledgeSharesQuery,
  ShareKnowledgeRequest,
} from "../../../store/api-slices/knowledgeBaseAPISlice";
import { User } from "../../../typings/component.props";
import { Group } from "../../../typings/request";
import { useTranslation } from "react-i18next";

interface ShareKnowledgeDialogProps {
  open: boolean;
  onClose: () => void;
  knowledgeBaseId: string;
}

interface ShareItem extends ShareKnowledgeRequest {
  name: string;
}

const ShareKnowledgeDialog = ({
  open,
  onClose,
  knowledgeBaseId,
}: ShareKnowledgeDialogProps) => {
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

  const { data: existingShares, isLoading: isLoadingShares } =
    useGetKnowledgeSharesQuery(knowledgeBaseId, { skip: !open });

  const [shareKnowledge, { isLoading: isSharing }] =
    useShareKnowledgeMutation();

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

  // Load existing shares into the local state
  useEffect(() => {
    if (existingShares && open) {
      const shares: ShareItem[] = existingShares.map((share) => ({
        target_id: share.resource_id,
        target_type: share.resource_type,
        role: share.role || "viewer",
        name: share.resource_name,
      }));
      setShareList(shares);
    }
  }, [existingShares, open]);

  const handleTabChange = (_event: React.SyntheticEvent, newValue: number) => {
    setTabValue(newValue);
    setSearchQuery("");
    setSelectedTarget(null);
  };

  const handleSelectUser = (user: User) => {
    const newTarget = {
      id: user.id,
      name: user.full_name || user.email,
      type: "user" as const,
    };
    setSelectedTarget(newTarget);
    // Clear search to show the selection form
    setSearchQuery("");
  };

  const handleSelectGroup = (group: Group) => {
    const newTarget = {
      id: group.id,
      name: group.name,
      type: "group" as const,
    };
    setSelectedTarget(newTarget);
    // Clear search to show the selection form
    setSearchQuery("");
  };

  const handleAddShare = () => {
    if (!selectedTarget) return;

    // Check if already in the list
    const exists = shareList.some(
      (share) => share.target_id === selectedTarget.id,
    );

    if (exists) {
      // Update existing share's role
      setShareList((prev) =>
        prev.map((share) =>
          share.target_id === selectedTarget.id ? { ...share, role } : share,
        ),
      );
    } else {
      // Add new share
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

    // Reset selection
    setSelectedTarget(null);
    setRole("viewer");
    setSearchQuery("");
  };

  const handleRemoveShare = (targetId: string) => {
    setShareList((prev) =>
      prev.filter((share) => share.target_id !== targetId),
    );
  };

  const handleSubmit = async () => {
    try {
      // Submit the entire share list
      const requestBody: ShareKnowledgeRequest[] = shareList.map((share) => ({
        target_id: share.target_id,
        target_type: share.target_type,
        role: share.role,
      }));

      await shareKnowledge({
        id: knowledgeBaseId,
        requestBody,
      }).unwrap();

      handleClose();
    } catch (error) {
      console.error("Failed to share knowledge base:", error);
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
      <DialogTitle>{t("Share Knowledge Base")}</DialogTitle>
      <DialogContent>
        {isLoadingShares ? (
          <Box
            sx={{
              display: "flex",
              justifyContent: "center",
              alignItems: "center",
              py: 4,
            }}
          >
            <CircularProgress />
          </Box>
        ) : (
          <>
            {/* Existing Shares Section */}
            <Box sx={{ mb: 3 }}>
              <Typography variant="subtitle1" sx={{ mb: 1, fontWeight: 600 }}>
                Current Shares ({shareList.length})
              </Typography>
              {shareList.length === 0 ? (
                <Typography variant="body2" color="text.secondary">
                  No shares yet. Add users or groups below.
                </Typography>
              ) : (
                <List
                  dense
                  sx={{ border: 1, borderColor: "divider", borderRadius: 1 }}
                >
                  {shareList.map((share, index) => (
                    <Box key={share.target_id}>
                      {index > 0 && <Divider />}
                      <ListItem
                        secondaryAction={
                          <IconButton
                            edge="end"
                            aria-label="delete"
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

            {/* Add New Share Section */}
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
                    bgcolor: "primary.light",
                    border: 2,
                    borderColor: "primary.main",
                    borderRadius: 1,
                  }}
                >
                  <Typography variant="body2" sx={{ mb: 2, fontWeight: 600 }}>
                    <strong>Selected:</strong> {selectedTarget.name} (
                    {selectedTarget.type})
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
                    size="medium"
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
        <Button onClick={handleClose}>{t("Cancel")}</Button>
        <Button onClick={handleSubmit} variant="contained" disabled={isSharing}>
          {isSharing ? t("Saving...") : t("Save Shares")}
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default ShareKnowledgeDialog;
