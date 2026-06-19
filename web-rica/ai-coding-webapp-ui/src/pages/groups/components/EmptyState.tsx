import Box from "@mui/material/Box";
import Typography from "@mui/material/Typography";
import Button from "@mui/material/Button";
import FolderIcon from "@mui/icons-material/Folder";

interface EmptyStateProps {
  onCreateClick: () => void;
}

const EmptyState = ({ onCreateClick }: EmptyStateProps) => {
  return (
    <Box
      sx={{
        display: "flex",
        flexDirection: "column",
        alignItems: "center",
        justifyContent: "center",
        height: "100%",
        textAlign: "center",
        py: 8,
      }}
    >
      <Box
        sx={{
          width: 120,
          height: 120,
          borderRadius: "50%",
          backgroundColor: "action.hover",
          display: "flex",
          alignItems: "center",
          justifyContent: "center",
          mb: 3,
        }}
      >
        <FolderIcon sx={{ fontSize: 60, color: "text.secondary" }} />
      </Box>
      <Typography variant="h5" gutterBottom color="text.primary">
        No Groups Yet
      </Typography>
      <Typography
        variant="body2"
        color="text.secondary"
        sx={{ mb: 3, maxWidth: 400 }}
      >
        You haven't created or joined any groups yet. Create your first group to
        start collaborating with your team.
      </Typography>
      <Button variant="contained" size="large" onClick={onCreateClick}>
        Create Your First Group
      </Button>
    </Box>
  );
};

export default EmptyState;
