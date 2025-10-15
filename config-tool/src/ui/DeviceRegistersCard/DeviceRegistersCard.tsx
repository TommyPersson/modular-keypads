import RefreshOutlinedIcon from "@mui/icons-material/RefreshOutlined"
import {
  Table,
  TableHead,
  TableRow,
  TableCell,
  TableBody,
  Card,
  CardContent,
  CardHeader,
  IconButton
} from "@mui/material"
import { useQuery } from "@tanstack/react-query"
import { useDeviceContext } from "../../context/DeviceContext"
import { GetDeviceRegisterValuesQuery } from "../../queries/GetDeviceRegisterValuesQuery"

export const DeviceRegistersCard = () => {
  const { facade: deviceFacade } = useDeviceContext()

  const deviceRegisterValuesQuery = useQuery(GetDeviceRegisterValuesQuery(deviceFacade))

  const deviceRegisterValues = deviceRegisterValuesQuery.data ?? {}

  const deviceRegisterValuesList = Object.entries(deviceRegisterValues).map(it => ({ name: it[0], value: it[1] }))

  console.log(deviceRegisterValues, deviceRegisterValuesList)

  return (
    <Card>
      <CardHeader
        title={"Device Registers"}
        subheader={"Last read at 13:37:00"}
        action={
          <>
            <IconButton
              onClick={() => deviceRegisterValuesQuery.refetch()}
              children={<RefreshOutlinedIcon />}
            />
          </>
        }
      />

      <CardContent>
        <Table size={"small"}>
          <TableHead>
            <TableRow>
              <TableCell style={{ width: 0 }}>Name</TableCell>
              <TableCell>Value</TableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            {
              deviceRegisterValuesList.map(it => (
                <TableRow key={it.name}>
                  <TableCell><code>{it.name}</code></TableCell>
                  <TableCell><code>0x{it.value.toString(16).padStart(2, '0')}</code></TableCell>
                </TableRow>
              ))
            }
          </TableBody>
        </Table>
      </CardContent>
    </Card>
  )
}
