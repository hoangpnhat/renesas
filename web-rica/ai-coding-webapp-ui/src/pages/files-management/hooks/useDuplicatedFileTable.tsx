import { useMemo } from "react";
import { GridColDef } from "@mui/x-data-grid";
import { useTranslation } from "react-i18next";

export const useDuplicatedFileTable = () => {
  const { t } = useTranslation();
  const dataSource: GridColDef[] = useMemo<GridColDef[]>(() => {
    return [
      {
        field: "file_name",
        headerName: t("table.header.file_name"),
        width: 550,
        renderCell: (params) => {
          return <>{params.value}</>;
        },
      },
      {
        field: "action",
        headerName: t("table.header.actions"),
        width: 250,
        renderCell: (params) => {
          return <>{params.value}</>;
        },
      },
    ];
  }, [t]);

  return { dataSource };
};
