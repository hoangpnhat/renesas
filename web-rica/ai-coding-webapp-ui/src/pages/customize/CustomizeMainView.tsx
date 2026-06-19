import { useState } from "react";
import Box from "@mui/material/Box";
import Typography from "@mui/material/Typography";
import TuneIcon from "@mui/icons-material/Tune";
import { useTranslation } from "react-i18next";
import {
  CustomizeItem,
  CustomizeListItem,
  CustomizeType,
  ShareCustomizeRequest,
} from "../../store/api-slices/promptApiSlice.ts";
import { SharedResource } from "../../store/api-slices/knowledgeBaseAPISlice.ts";
import { CustomizeListPanel } from "./components/CustomizeListPanel.tsx";
import { CustomizeDetailPanel } from "./components/CustomizeDetailPanel.tsx";
import { CustomizeFormModal } from "./components/CustomizeFormModal.tsx";
import ShareCustomizeDialog from "./components/ShareCustomizeDialog.tsx";

export interface CustomizeMainViewProps {
  type: CustomizeType;
  // List
  items: CustomizeListItem[];
  isFetching: boolean;
  onSearch: (query: string) => void;
  // Selection
  selectedItem: CustomizeItem | null;
  onSelect: (item: CustomizeListItem | null) => void;
  // Create
  isCreating: boolean;
  onCreateSubmit: (formData: FormData) => Promise<void>;
  // Update
  isUpdating: boolean;
  onUpdateSubmit: (formData: FormData) => Promise<void>;
  // Delete
  onDelete: () => Promise<void>;
  // Share
  shareOpen: boolean;
  onShareOpen: () => void;
  onShareClose: () => void;
  shares: SharedResource[];
  isLoadingShares: boolean;
  isSharing: boolean;
  onShare: (requestBody: ShareCustomizeRequest[]) => Promise<void>;
}

const CustomizeMainView = ({
  type,
  items,
  isFetching,
  onSearch,
  selectedItem,
  onSelect,
  isCreating,
  onCreateSubmit,
  isUpdating,
  onUpdateSubmit,
  onDelete,
  shareOpen,
  onShareOpen,
  onShareClose,
  shares,
  isLoadingShares,
  isSharing,
  onShare,
}: CustomizeMainViewProps) => {
  const { t } = useTranslation();
  const [createOpen, setCreateOpen] = useState(false);
  const [editOpen, setEditOpen] = useState(false);

  return (
    <Box
      sx={{
        display: "flex",
        height: "100%",
        overflow: "hidden",
        bgcolor: "background.default",
      }}
    >
      {/* List */}
      <CustomizeListPanel
        type={type}
        items={items}
        isFetching={isFetching}
        selectedId={selectedItem?.id ?? null}
        onSelect={onSelect}
        onSearch={onSearch}
        onCreateOpen={() => setCreateOpen(true)}
      />

      {/* Detail */}
      <Box sx={{ flex: 1, display: "flex", overflow: "hidden" }}>
        {selectedItem ? (
          <CustomizeDetailPanel
            key={selectedItem.id}
            item={selectedItem}
            onEdit={() => setEditOpen(true)}
            onShare={onShareOpen}
            onDelete={onDelete}
          />
        ) : (
          <Box
            sx={{
              flex: 1,
              display: "flex",
              flexDirection: "column",
              alignItems: "center",
              justifyContent: "center",
              gap: 1.5,
            }}
          >
            <TuneIcon sx={{ fontSize: 48, color: "text.disabled" }} />
            <Typography sx={{ fontSize: "14px", color: "text.secondary" }}>
              {t("customize.selectToView")}
            </Typography>
          </Box>
        )}
      </Box>

      {/* Create modal */}
      <CustomizeFormModal
        open={createOpen}
        onClose={() => setCreateOpen(false)}
        type={type}
        isSubmitting={isCreating}
        onSubmit={onCreateSubmit}
      />

      {/* Edit modal */}
      <CustomizeFormModal
        open={editOpen}
        onClose={() => setEditOpen(false)}
        type={type}
        item={selectedItem ?? undefined}
        isSubmitting={isUpdating}
        onSubmit={onUpdateSubmit}
      />

      {/* Share dialog */}
      {selectedItem && (
        <ShareCustomizeDialog
          open={shareOpen}
          onClose={onShareClose}
          title={selectedItem.name}
          shares={shares}
          isLoadingShares={isLoadingShares}
          isSharing={isSharing}
          onShare={onShare}
        />
      )}
    </Box>
  );
};

export default CustomizeMainView;
