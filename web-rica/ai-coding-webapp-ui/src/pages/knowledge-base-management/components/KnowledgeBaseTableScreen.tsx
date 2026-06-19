/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { Fragment, useState } from "react";
import { CommonToolbar } from "../../../components/CommonToolbar.tsx";

import CommonTableView from "../../../components/table/CommonTableView.tsx";

import { useCustomPagination } from "../../../hooks/useCustomPagination.tsx";
import { useGetKnowledgeBasesQuery } from "../../../store/api-slices/knowledgeBaseAPISlice.ts";
import { useKnowledgeBaseDataTable } from "../hooks/useKnowledgeBaseDataTable.tsx";
import { useKnowledgeBaseToolBarItems } from "../hooks/useKnowledgeBaseToolBarItems.tsx";
import { useGetFilesQuery } from "../../../store/api-slices/fileApiSlice.ts";
import { ENTRIES_LARGE_RECORDS } from "../../../constants/common.ts";
import ShareKnowledgeDialog from "./ShareKnowledgeDialog.tsx";

const KnowledgeBaseTableScreen = () => {
  const [shareDialogOpen, setShareDialogOpen] = useState(false);
  const [selectedKnowledgeBaseId, setSelectedKnowledgeBaseId] = useState<
    string | null
  >(null);

  const {
    page,
    entries,
    handleChangePage,
    handleChangeRowsPerPage,
    query,
    onChangeInput,
    trueQuery,
  } = useCustomPagination(0, 10);

  // Fetch regular knowledge bases
  const { data: enquiryItems, isFetching: loading } = useGetKnowledgeBasesQuery(
    {
      page,
      entries: entries,
      query: query || "",
    },
    {
      skip: entries <= 0 || page < 0,
    },
  );

  const { data: fileList } = useGetFilesQuery({
    page: 0,
    entries: ENTRIES_LARGE_RECORDS,
  });

  const handleShareClick = (knowledgeBaseId: string) => {
    setSelectedKnowledgeBaseId(knowledgeBaseId);
    setShareDialogOpen(true);
  };

  const handleCloseShareDialog = () => {
    setShareDialogOpen(false);
    setSelectedKnowledgeBaseId(null);
  };

  const toolbarItems = useKnowledgeBaseToolBarItems(
    trueQuery,
    fileList?.data || [],
    onChangeInput,
  );
  const { dataSource, ref, getRowClassName } = useKnowledgeBaseDataTable(
    fileList?.data || [],
    handleShareClick,
  );

  return (
    <Fragment>
      <CommonToolbar
        title="Knowledge Base"
        isUploadEnabled={true}
        items={toolbarItems}
        externalSX={{
          height: "18%",
          marginLeft: "0.75rem",
          marginTop: "0.5rem",
        }}
      />
      <CommonTableView
        ref={ref}
        loading={loading}
        columns={dataSource}
        rows={enquiryItems?.data}
        getRowClassName={getRowClassName}
        pagination={{
          page,
          entries,
          handleChangePage,
          handleChangeRowsPerPage,
          total: enquiryItems?.pagination.total || 0,
        }}
      />
      {selectedKnowledgeBaseId && (
        <ShareKnowledgeDialog
          open={shareDialogOpen}
          onClose={handleCloseShareDialog}
          knowledgeBaseId={selectedKnowledgeBaseId}
        />
      )}
    </Fragment>
  );
};

export default KnowledgeBaseTableScreen;
