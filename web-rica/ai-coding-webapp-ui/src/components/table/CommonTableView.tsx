/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import React, { forwardRef, Fragment } from "react";
import { Box, TablePagination } from "@mui/material";
import {
  DataGrid,
  DataGridProps,
  GridColDef,
  GridLoadingOverlay,
} from "@mui/x-data-grid";
import { PaginationProps } from "../../typings/common.props.ts";
import CustomNoRowsOverlay from "./CustomOverlayTable.tsx";

// Định nghĩa interface cho props của component với generic T
export interface CommonTableViewProps<T>
  extends PaginationProps,
    Omit<DataGridProps, "pagination"> {
  columns: GridColDef[];
  rows?: T[];
  loading: boolean;
  hasNoRowOverlayDisplay?: boolean;
}

// Định nghĩa inner component có generic T, nhận ref kiểu HTMLDivElement
function CommonTableViewInner<T>(
  {
    columns,
    loading,
    rows,
    hasNoRowOverlayDisplay = true,
    pagination: {
      page,
      entries,
      handleChangePage,
      handleChangeRowsPerPage,
      total,
    },
    ...props
  }: CommonTableViewProps<T>,
  ref: React.Ref<HTMLDivElement>,
): JSX.Element {
  return (
    <Fragment>
      <Box
        ref={ref}
        sx={{
          height: "70%",
          maxWidth: "100%",
          p: 3,
          bgcolor: "background.paper",
        }}
      >
        <DataGrid
          {...props}
          rows={rows}
          columns={columns}
          hideFooter={true}
          loading={loading}
          sx={(theme) => ({
            border: `1px solid ${theme.palette.divider}`,
            color: theme.palette.text.primary,
            "& .MuiDataGrid-columnHeaders": {
              backgroundColor: theme.palette.background.default,
              color: theme.palette.text.primary,
              borderBottom: `1px solid ${theme.palette.divider}`,
            },
            "& .MuiDataGrid-columnHeaderTitle": {
              fontWeight: 600,
              fontSize: "0.8125rem",
            },
            "& .MuiDataGrid-cell": {
              color: theme.palette.text.primary,
              fontSize: "0.8125rem",
              borderColor: theme.palette.divider,
            },
            "& .MuiDataGrid-row": {
              "&:nth-of-type(even)": {
                backgroundColor: theme.palette.background.default,
              },
              "&:hover": {
                backgroundColor: theme.palette.action.hover,
              },
            },
            "& .MuiDataGrid-footerContainer": {
              borderColor: theme.palette.divider,
              color: theme.palette.text.primary,
            },
            "& .MuiDataGrid-virtualScroller": {
              overflowY: "scroll",
            },
            "& .MuiTablePagination-root": {
              color: theme.palette.text.primary,
            },
          })}
          slotProps={
            hasNoRowOverlayDisplay
              ? {
                  loadingOverlay: {
                    variant: "skeleton",
                    noRowsVariant: "skeleton",
                  },
                }
              : {}
          }
          slots={{
            noRowsOverlay: CustomNoRowsOverlay,
            loadingOverlay: hasNoRowOverlayDisplay
              ? GridLoadingOverlay
              : () => null,
          }}
        />
      </Box>
      <TablePagination
        component="div"
        sx={(theme) => ({
          height: "10%",
          color: theme.palette.text.primary,
          borderTop: `1px solid ${theme.palette.divider}`,
          "& .MuiTablePagination-selectIcon": {
            color: theme.palette.text.secondary,
          },
          "& .MuiTablePagination-actions button": {
            color: theme.palette.text.primary,
          },
        })}
        className="pagination-page"
        count={total}
        page={page}
        rowsPerPage={entries}
        showFirstButton
        showLastButton
        onRowsPerPageChange={handleChangeRowsPerPage}
        onPageChange={handleChangePage}
      />
    </Fragment>
  );
}

// Sử dụng forwardRef và ép kiểu để tạo component generic
const CommonTableView = forwardRef(CommonTableViewInner) as <T>(
  props: CommonTableViewProps<T> & { ref?: React.Ref<HTMLDivElement> },
) => JSX.Element;

export default CommonTableView;
