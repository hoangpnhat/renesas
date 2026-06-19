import { useCustomPagination } from "../../../hooks/useCustomPagination.tsx";
import { Fragment } from "react";
import { useFileDataTable } from "../hooks/useFileDataTable.tsx";
import { useGetFilesQuery } from "../../../store/api-slices/fileApiSlice.ts";
import { useFileToolbarItem } from "../hooks/useFileToolbarItem.tsx";
import { CommonToolbar } from "../../../components/CommonToolbar.tsx";
import CommonTableView from "../../../components/table/CommonTableView.tsx";
import FilePreviewModal from "./FilePreviewModal.tsx";

const FileTableScreen = () => {
  const {
    page,
    entries,
    handleChangePage,
    handleChangeRowsPerPage,
    query,
    onChangeInput,
    trueQuery,
  } = useCustomPagination(0, 10);
  const { data: enquiryItems, isFetching: loading } = useGetFilesQuery(
    { page, entries, query },
    { skip: entries <= 0 },
  );
  const { dataSource, ref, previewFile, setPreviewFile } = useFileDataTable();

  // useEffect(() => {
  //   getStatus();
  //   const interval = setInterval(() => {
  //     getStatus();
  //   }, 30000);

  //   return () => clearInterval(interval);
  // }, []);

  // const isUploadEnabled = useMemo(() => {
  //   return true;
  //   // return status?.status === "warm";
  // }, [status?.status]);

  const toolbarItems = useFileToolbarItem(trueQuery, onChangeInput, true);
  return (
    <Fragment>
      <CommonToolbar
        title="Files Management"
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
        pagination={{
          page,
          entries,
          handleChangePage,
          handleChangeRowsPerPage,
          total: enquiryItems?.pagination.total || 0,
        }}
      />
      <FilePreviewModal
        fileId={previewFile?.id ?? null}
        fileName={previewFile?.file_name ?? ""}
        isOpen={previewFile !== null}
        onClose={() => setPreviewFile(null)}
      />
    </Fragment>
  );
};

export default FileTableScreen;
