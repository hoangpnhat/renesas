import Button from "@mui/material/Button";
import Paper from "@mui/material/Paper";
import Table from "@mui/material/Table";
import TableBody from "@mui/material/TableBody";
import TableCell from "@mui/material/TableCell";
import TableContainer from "@mui/material/TableContainer";
import TableHead from "@mui/material/TableHead";
import TableRow from "@mui/material/TableRow";
import { Fragment } from "react/jsx-runtime";
import { CommonTableProps } from "../typings/component.props";
import { BaseObject } from "../typings";

export const CommonTable = <T,>({
  listColumnNames,
  displayColumns,
  itemsRows,
}: CommonTableProps<T>) => {
  return (
    <Fragment>
      <TableContainer component={Paper} className="mb-8">
        <Table aria-label="company rules table">
          <TableHead>
            <TableRow>
              {listColumnNames.map((element) => (
                <TableCell key={element}>{displayColumns[element]}</TableCell>
              ))}
            </TableRow>
          </TableHead>
          <TableBody>
            {(itemsRows as Array<T>).map((row, rowId) => (
              <TableRow key={rowId}>
                {listColumnNames.map((element) => (
                  <TableCell key={element + rowId}>
                    {element !== "actions" ? (
                      (row as BaseObject)[element]
                    ) : (
                      <Button
                        variant="contained"
                        onClick={() => (row as any)["actions"](row)}
                      >
                        Prompt with this rule
                      </Button>
                    )}
                  </TableCell>
                ))}
              </TableRow>
            ))}
          </TableBody>
        </Table>
      </TableContainer>
    </Fragment>
  );
};
