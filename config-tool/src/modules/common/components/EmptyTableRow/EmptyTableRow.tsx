import { TableCell, TableRow } from "@mui/material"

export type EmptyTableRowProps = {
  colSpan?: number
  children: any
}

export const EmptyTableRow = (props: EmptyTableRowProps) => {
  return (
    <TableRow>
      <TableCell colSpan={props.colSpan ?? 1000}>
        <center>
          <em>{props.children}</em>
        </center>
      </TableCell>
    </TableRow>
  )
}