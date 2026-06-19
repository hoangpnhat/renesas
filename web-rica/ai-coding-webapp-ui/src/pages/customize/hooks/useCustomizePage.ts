import { useEffect, useState } from "react";
import {
  CustomizeListItem,
  CustomizeType,
  ShareCustomizeRequest,
  useCreateCustomizeMutation,
  useDeleteCustomizeMutation,
  useGetCustomizeDetailQuery,
  useGetCustomizeSharesQuery,
  useGetCustomizesQuery,
  useShareCustomizeMutation,
  useUpdateCustomizeMutation,
} from "../../../store/api-slices/promptApiSlice.ts";

export const useCustomizePage = (type: CustomizeType) => {
  const [searchQuery, setSearchQuery] = useState("");
  const [selectedListItem, setSelectedListItem] =
    useState<CustomizeListItem | null>(null);
  const [shareOpen, setShareOpen] = useState(false);

  const { data, isFetching } = useGetCustomizesQuery({
    page: 0,
    entries: 50,
    query: searchQuery,
    prompt_type: type,
  });

  const items = data?.data ?? [];

  // Auto-select first item when list loads or selection becomes invalid
  useEffect(() => {
    if (isFetching) return;
    if (items.length === 0) {
      setSelectedListItem(null);
      return;
    }
    if (!selectedListItem || !items.find((i) => i.id === selectedListItem.id)) {
      setSelectedListItem(items[0]);
    }
  }, [items, isFetching, selectedListItem]);

  const { data: selectedItem = null } = useGetCustomizeDetailQuery(
    selectedListItem?.id ?? "",
    { skip: !selectedListItem },
  );

  const [createCustomize, { isLoading: isCreating }] =
    useCreateCustomizeMutation();
  const [updateCustomize, { isLoading: isUpdating }] =
    useUpdateCustomizeMutation();
  const [deleteCustomize] = useDeleteCustomizeMutation();

  const { data: shares = [], isLoading: isLoadingShares } =
    useGetCustomizeSharesQuery(selectedListItem?.id ?? "", {
      skip: !shareOpen || !selectedListItem,
    });

  const [shareCustomize, { isLoading: isSharing }] =
    useShareCustomizeMutation();

  const onCreateSubmit = async (formData: FormData) => {
    // add prompt_type to formData
    formData.append("prompt_type", type);
    // change binary file field name to "prompt_file"
    const file = formData.get("file");
    if (file) {
      formData.delete("file");
      formData.append("prompt_file", file);
    }
    await createCustomize(formData).unwrap();
  };

  const onUpdateSubmit = async (formData: FormData) => {
    if (!selectedListItem) return;
    await updateCustomize({ id: selectedListItem.id, formData }).unwrap();
  };

  const onDelete = async () => {
    if (!selectedListItem) return;
    await deleteCustomize(selectedListItem.id).unwrap();
    // selection will be corrected by the useEffect above once the list refreshes
    setSelectedListItem(null);
  };

  const onShare = async (requestBody: ShareCustomizeRequest[]) => {
    if (!selectedListItem) return;
    await shareCustomize({ id: selectedListItem.id, requestBody }).unwrap();
  };

  return {
    items,
    isFetching,
    selectedItem,
    selectedListItemId: selectedListItem?.id ?? null,
    onSelect: (item: CustomizeListItem | null) => setSelectedListItem(item),
    onSearch: setSearchQuery,
    shareOpen,
    onShareOpen: () => setShareOpen(true),
    onShareClose: () => setShareOpen(false),
    shares,
    isLoadingShares,
    isSharing,
    isCreating,
    isUpdating,
    onCreateSubmit,
    onUpdateSubmit,
    onDelete,
    onShare,
  };
};
