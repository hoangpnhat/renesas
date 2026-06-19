import { useState, useEffect, useRef, useCallback } from "react";
import { useNavigate } from "react-router-dom";
import Box from "@mui/material/Box";
import Typography from "@mui/material/Typography";
import Button from "@mui/material/Button";
import Card from "@mui/material/Card";
import CardContent from "@mui/material/CardContent";
import Grid from "@mui/material/Grid";
import Chip from "@mui/material/Chip";
import CircularProgress from "@mui/material/CircularProgress";
import { SimpleTreeView } from "@mui/x-tree-view/SimpleTreeView";
import { TreeItem } from "@mui/x-tree-view/TreeItem";
import FolderIcon from "@mui/icons-material/Folder";
import DescriptionIcon from "@mui/icons-material/Description";
import { useGetGroupsQuery } from "../../store/api-slices/groupApiSlice";
import CreateGroupDialog from "./components/CreateGroupDialog";
import EmptyState from "./components/EmptyState";
import LoadingState from "./components/LoadingState";
import { useTranslation } from "react-i18next";

const GroupListView = () => {
  const navigate = useNavigate();
  const { t } = useTranslation();
  const [openCreate, setOpenCreate] = useState(false);
  const { data: groupsData, isLoading } = useGetGroupsQuery({
    page: 0,
    entries: 20,
  });
  const [displayedGroups, setDisplayedGroups] = useState<any[]>([]);
  const [hasMore, setHasMore] = useState(true);
  const [isLoadingMore, setIsLoadingMore] = useState(false);
  const loadMoreTriggerRef = useRef<HTMLDivElement>(null);

  // Initialize displayedGroups when groupsData loads
  useEffect(() => {
    if (groupsData?.data) {
      setDisplayedGroups(groupsData.data.slice(0, 5));
    }
  }, [groupsData]);

  const handleGroupClick = (groupId: string) => {
    navigate(`/management/groups/${groupId}`);
  };

  const loadMore = useCallback(() => {
    if (isLoadingMore || !hasMore) return;

    setIsLoadingMore(true);

    // Simulate API delay
    setTimeout(() => {
      if (!groupsData) {
        setIsLoadingMore(false);
        setHasMore(false);
        return;
      }

      const itemsPerPage = 3;
      const currentLength = displayedGroups.length;
      const startIndex = currentLength;
      const endIndex = startIndex + itemsPerPage;
      const newGroups = groupsData.data.slice(startIndex, endIndex);

      if (newGroups.length > 0) {
        setDisplayedGroups((prev) => [...prev, ...newGroups]);

        // Check if we've loaded all groups
        if (currentLength + newGroups.length >= groupsData.pagination.total) {
          setHasMore(false);
        }
      } else {
        setHasMore(false);
      }

      setIsLoadingMore(false);
    }, 600);
  }, [hasMore, isLoadingMore, displayedGroups.length, groupsData]);

  useEffect(() => {
    const trigger = loadMoreTriggerRef.current;
    if (!trigger) return;

    const observer = new IntersectionObserver(
      (entries) => {
        const firstEntry = entries[0];
        if (firstEntry.isIntersecting && hasMore && !isLoadingMore) {
          loadMore();
        }
      },
      {
        root: null, // viewport
        rootMargin: "100px", // Start loading 100px before the trigger element
        threshold: 0.1,
      },
    );

    observer.observe(trigger);

    return () => {
      if (trigger) {
        observer.unobserve(trigger);
      }
    };
  }, [loadMore, hasMore, isLoadingMore]);

  if (isLoading) {
    return <LoadingState />;
  }

  return (
    <Box
      sx={{ p: 3, height: "100vh", display: "flex", flexDirection: "column" }}
    >
      <Box sx={{ display: "flex", justifyContent: "space-between", mb: 3 }}>
        <Typography variant="h4">{t("Groups")}</Typography>
        <Button variant="contained" onClick={() => setOpenCreate(true)}>
          {t("Create Group")}
        </Button>
      </Box>

      <Box
        sx={{
          flex: 1,
          overflow: "auto",
          pr: 1,
        }}
      >
        {displayedGroups.length === 0 && !isLoadingMore ? (
          <EmptyState onCreateClick={() => setOpenCreate(true)} />
        ) : (
          <>
            <Grid container spacing={2}>
              {displayedGroups.map((group) => (
                <Grid item xs={12} key={group.id}>
                  <Card sx={{ "&:hover": { boxShadow: 4 } }}>
                    <CardContent
                      sx={{ py: 1.5, px: 2, "&:last-child": { pb: 1.5 } }}
                    >
                      <Box
                        sx={{ cursor: "pointer" }}
                        onClick={() => handleGroupClick(group.id)}
                      >
                        <Typography
                          variant="body1"
                          fontWeight={500}
                          gutterBottom
                        >
                          {group.name}
                        </Typography>
                        <Typography
                          variant="caption"
                          color="text.secondary"
                          sx={{ mb: 0.5, display: "block" }}
                        >
                          {group.description || "No description"}
                        </Typography>
                        <Box
                          sx={{
                            display: "flex",
                            gap: 0.5,
                            flexWrap: "wrap",
                            mb: 1,
                          }}
                        >
                          <Chip
                            label={group.visibility}
                            size="small"
                            color={
                              group.visibility === "private"
                                ? "error"
                                : "success"
                            }
                            sx={{ height: 20, fontSize: "0.7rem" }}
                          />
                          <Chip
                            label={`Members: ${group.members?.length || 0}`}
                            size="small"
                            color="primary"
                            sx={{ height: 20, fontSize: "0.7rem" }}
                          />
                        </Box>
                      </Box>

                      {(group as any).resources &&
                        (group as any).resources.length > 0 && (
                          <SimpleTreeView
                            onClick={(e) => e.stopPropagation()}
                            sx={{ mt: 1, fontSize: "0.75rem" }}
                          >
                            <TreeItem
                              itemId={`resources-${group.id}`}
                              label={
                                <Box
                                  sx={{
                                    display: "flex",
                                    alignItems: "center",
                                    gap: 0.5,
                                  }}
                                >
                                  <FolderIcon sx={{ fontSize: 16 }} />
                                  <Typography
                                    variant="caption"
                                    fontWeight={500}
                                  >
                                    Resources ({(group as any).resources.length}
                                    )
                                  </Typography>
                                </Box>
                              }
                            >
                              {(group as any).resources.map(
                                (resource: any, index: number) => (
                                  <TreeItem
                                    key={`${group.id}-resource-${index}`}
                                    itemId={`${group.id}-resource-${index}`}
                                    label={
                                      <Box
                                        sx={{
                                          display: "flex",
                                          alignItems: "center",
                                          gap: 0.5,
                                          py: 0.25,
                                        }}
                                      >
                                        <DescriptionIcon
                                          sx={{ fontSize: 14 }}
                                        />
                                        <Typography variant="caption">
                                          {resource.resource_name}
                                        </Typography>
                                        <Box
                                          sx={{
                                            display: "flex",
                                            alignItems: "center",
                                            gap: 0.25,
                                            ml: "auto",
                                          }}
                                        >
                                          <Typography
                                            variant="caption"
                                            color="text.secondary"
                                            sx={{ fontSize: "0.65rem" }}
                                          >
                                            {resource.resource_type}
                                          </Typography>
                                        </Box>
                                      </Box>
                                    }
                                  />
                                ),
                              )}
                            </TreeItem>
                          </SimpleTreeView>
                        )}
                    </CardContent>
                  </Card>
                </Grid>
              ))}
            </Grid>

            {/* Intersection Observer trigger element */}
            <Box
              ref={loadMoreTriggerRef}
              sx={{ py: 2, textAlign: "center", minHeight: "20px" }}
            >
              {isLoadingMore && <CircularProgress size={30} />}
            </Box>

            {/* End of list message */}
            {!hasMore && displayedGroups.length > 0 && (
              <Box sx={{ py: 2, textAlign: "center" }}>
                <Typography variant="caption" color="text.secondary">
                  {t("No more groups to load")}
                </Typography>
              </Box>
            )}
          </>
        )}
      </Box>

      <CreateGroupDialog
        open={openCreate}
        onClose={() => setOpenCreate(false)}
      />
    </Box>
  );
};

export default GroupListView;
