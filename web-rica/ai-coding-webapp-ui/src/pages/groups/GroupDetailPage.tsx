import { useParams, useNavigate } from "react-router-dom";
import Box from "@mui/material/Box";
import Typography from "@mui/material/Typography";
import Button from "@mui/material/Button";
import Card from "@mui/material/Card";
import CardContent from "@mui/material/CardContent";
import Chip from "@mui/material/Chip";
import CircularProgress from "@mui/material/CircularProgress";
import { SimpleTreeView } from "@mui/x-tree-view/SimpleTreeView";
import { TreeItem } from "@mui/x-tree-view/TreeItem";
import FolderIcon from "@mui/icons-material/Folder";
import DescriptionIcon from "@mui/icons-material/Description";
import AccessTimeIcon from "@mui/icons-material/AccessTime";
import {
  useGetGroupQuery,
  useDeleteGroupMutation,
} from "../../store/api-slices/groupApiSlice";
import EditGroupDialog from "./components/EditGroupDialog";
import InviteMembersDialog from "./components/InviteMembersDialog";
import { useState } from "react";

const GroupDetailPage = () => {
  const { groupId } = useParams<{ groupId: string }>();
  const navigate = useNavigate();
  const { data: group, isLoading } = useGetGroupQuery(groupId || "");
  const [deleteGroup] = useDeleteGroupMutation();

  const [openEdit, setOpenEdit] = useState(false);
  const [openInvite, setOpenInvite] = useState(false);

  const handleDelete = async () => {
    if (window.confirm("Are you sure you want to delete this group?")) {
      try {
        await deleteGroup(groupId || "").unwrap();
        navigate("/management/groups");
      } catch (error) {
        console.error("Failed to delete group:", error);
      }
    }
  };

  if (isLoading) {
    return (
      <Box
        sx={{
          display: "flex",
          justifyContent: "center",
          alignItems: "center",
          height: "100vh",
        }}
      >
        <CircularProgress />
      </Box>
    );
  }

  if (!group) {
    return (
      <Box sx={{ p: 3 }}>
        <Typography variant="h5">Group not found</Typography>
      </Box>
    );
  }

  const canPerformCriticalActions = group.is_owner || group.is_admin || false;

  return (
    <Box sx={{ p: 3 }}>
      <Box
        sx={{
          display: "flex",
          justifyContent: "space-between",
          alignItems: "center",
          mb: 3,
        }}
      >
        <Typography variant="h4">{group.name}</Typography>
        <Box sx={{ display: "flex", gap: 1 }}>
          {canPerformCriticalActions && (
            <>
              <Button variant="outlined" onClick={() => setOpenInvite(true)}>
                Manage Members
              </Button>
              <Button variant="outlined" onClick={() => setOpenEdit(true)}>
                Edit
              </Button>
              <Button variant="outlined" color="error" onClick={handleDelete}>
                Delete
              </Button>
            </>
          )}
          <Button variant="outlined" onClick={() => navigate("/management/groups")}>
            Back to Groups
          </Button>
        </Box>
      </Box>

      <Box
        sx={{
          maxHeight: "calc(100vh - 100px)",
          overflowY: "auto",
          pr: 1,
        }}
      >
        <Card sx={{ mb: 3 }}>
          <CardContent>
            <Typography variant="h6" gutterBottom>
              Details
            </Typography>
            <Typography variant="body1" paragraph>
              {group.description || "No description"}
            </Typography>
            <Box sx={{ display: "flex", gap: 1, mt: 2 }}>
              <Chip
                label={group.visibility}
                color={group.visibility === "private" ? "error" : "success"}
              />
              <Chip
                label={`Owner: ${group.owner_name || "Unknown"}`}
                color="primary"
              />
              <Chip
                label={`Created: ${new Date(group.created_at).toLocaleDateString()}`}
                color="default"
              />
            </Box>
          </CardContent>
        </Card>

        <Card sx={{ mb: 3 }}>
          <CardContent>
            <Typography variant="h6" gutterBottom>
              Members
            </Typography>
            {group.members && group.members.length > 0 ? (
              <SimpleTreeView defaultExpandedItems={["members-root"]}>
                <TreeItem
                  itemId="members-root"
                  label={
                    <Box sx={{ display: "flex", alignItems: "center", gap: 1 }}>
                      <FolderIcon />
                      <Typography variant="body1">
                        All Members ({group.members.length})
                      </Typography>
                    </Box>
                  }
                >
                  {group.members.map((member) => (
                    <TreeItem
                      key={member.user_id}
                      itemId={member.user_id}
                      label={
                        <Box
                          sx={{
                            display: "flex",
                            alignItems: "center",
                            gap: 1,
                            py: 1,
                          }}
                        >
                          <Box
                            sx={{
                              flex: 1,
                              display: "flex",
                              gap: 1,
                              alignItems: "center",
                            }}
                          >
                            <Typography variant="body2">
                              {member.full_name}
                            </Typography>
                            <Chip
                              label={member.joined_at ? member.role : "Pending"}
                              size="small"
                              color={
                                !member.joined_at
                                  ? "warning"
                                  : member.role === "admin"
                                    ? "primary"
                                    : "default"
                              }
                            />
                          </Box>
                        </Box>
                      }
                    />
                  ))}
                </TreeItem>
              </SimpleTreeView>
            ) : (
              <Typography variant="body2" color="text.secondary">
                No members available
              </Typography>
            )}
          </CardContent>
        </Card>

        <Card>
          <CardContent>
            <Typography variant="h6" gutterBottom>
              Resources
            </Typography>
            {group.resources && group.resources.length > 0 ? (
              <SimpleTreeView defaultExpandedItems={["resources-root"]}>
                <TreeItem
                  itemId="resources-root"
                  label={
                    <Box sx={{ display: "flex", alignItems: "center", gap: 1 }}>
                      <FolderIcon />
                      <Typography variant="body1">
                        All Resources ({group.resources.length})
                      </Typography>
                    </Box>
                  }
                >
                  {group.resources.map((resource, index) => (
                    <TreeItem
                      key={`resource-${resource.resource_id || index}`}
                      itemId={`resource-${resource.resource_id || index}`}
                      label={
                        <Box
                          sx={{
                            display: "flex",
                            alignItems: "center",
                            gap: 1,
                            py: 1,
                          }}
                        >
                          <DescriptionIcon />
                          <Box sx={{ flex: 1 }}>
                            <Typography variant="body2">
                              <Typography
                                component="span"
                                sx={{ fontWeight: 700 }}
                              >
                                {resource.resource_type[0]?.toUpperCase() +
                                  resource.resource_type.slice(1)}
                              </Typography>{" "}
                              {resource.resource_name}
                            </Typography>
                            <Box
                              sx={{
                                display: "flex",
                                alignItems: "center",
                                gap: 0.5,
                                mt: 0.5,
                              }}
                            >
                              <AccessTimeIcon
                                sx={{ fontSize: 14, color: "text.secondary" }}
                              />
                              <Typography
                                variant="caption"
                                color="text.secondary"
                              >
                                Last modified:{" "}
                                {new Date(
                                  resource.last_modified,
                                ).toLocaleString()}
                              </Typography>
                            </Box>
                          </Box>
                        </Box>
                      }
                    />
                  ))}
                </TreeItem>
              </SimpleTreeView>
            ) : (
              <Typography variant="body2" color="text.secondary">
                No resources available
              </Typography>
            )}
          </CardContent>
        </Card>
      </Box>

      {canPerformCriticalActions && (
        <>
          <EditGroupDialog
            open={openEdit}
            onClose={() => setOpenEdit(false)}
            group={group}
          />
          <InviteMembersDialog
            open={openInvite}
            onClose={() => setOpenInvite(false)}
            groupId={group.id}
            currentMembers={
              group.members as {
                user_id: string;
                role: string;
                full_name: string;
              }[]
            }
          />
        </>
      )}
    </Box>
  );
};

export default GroupDetailPage;
