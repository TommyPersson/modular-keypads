import RefreshOutlinedIcon from "@mui/icons-material/RefreshOutlined"
import {
  Card,
  CardContent,
  CardHeader,
  IconButton,
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableRow
} from "@mui/material"
import { useDeviceContext } from "@src/modules/device/context"
import { ListConnectedDevicesQuery } from "@src/modules/device/queries/ListConnectedDevicesQuery"
import { useQuery } from "@tanstack/react-query"

export const ConnectedDevicesCard = () => {
  const { facade: deviceFacade } = useDeviceContext()

  const query = useQuery(ListConnectedDevicesQuery(deviceFacade))

  const connectedDevices = query.data ?? []

  return (
    <Card>
      <CardHeader
        title={"Connected Devices"}
        action={
          <>
            <IconButton
              onClick={() => query.refetch()}
              children={<RefreshOutlinedIcon />}
            />
          </>
        }
      />

      <CardContent>
        <Table size={"small"}>
          <TableHead>
            <TableRow>
              <TableCell style={{ width: 0 }}>ID</TableCell>
              <TableCell>Address</TableCell>
              <TableCell>Name</TableCell>
              <TableCell>Type</TableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            {
              connectedDevices.map(it => (
                <TableRow key={it.deviceId}>
                  <TableCell><code>{it.deviceId}</code></TableCell>
                  <TableCell><code>0x{it.deviceAddress.toString(16).padStart(2, '0')}</code></TableCell>
                  <TableCell>{it.deviceName}</TableCell>
                  <TableCell><code>{it.deviceType}</code></TableCell>
                </TableRow>
              ))
            }
          </TableBody>
        </Table>
      </CardContent>
    </Card>
  )
}
