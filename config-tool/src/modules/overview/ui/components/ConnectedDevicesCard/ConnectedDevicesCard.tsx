import { Card, CardContent, CardHeader, Table, TableBody, TableCell, TableHead, TableRow } from "@mui/material"
import { CommandButton } from "@src/modules/common/components"
import { FlashDeviceIdentificationLightsCommand } from "@src/modules/device/commands/FlashDeviceIdentificationLightsCommand"
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
          </>
        }
      />

      <CardContent style={{ paddingLeft: 0, paddingRight: 0 }}>
        <Table size={"small"}>
          <TableHead>
            <TableRow>
              <TableCell>Name</TableCell>
              <TableCell style={{ width: 0 }}>ID</TableCell>
              <TableCell>Type</TableCell>
              <TableCell>Address</TableCell>
              <TableCell></TableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            {
              connectedDevices.map(it => (
                <TableRow key={it.deviceId}>
                  <TableCell>{it.deviceName}</TableCell>
                  <TableCell><code>{it.deviceId}</code></TableCell>
                  <TableCell><code>{it.deviceType}</code></TableCell>
                  <TableCell><code>{it.deviceAddress}</code></TableCell>
                  <TableCell>
                    <CommandButton
                      command={FlashDeviceIdentificationLightsCommand}
                      args={{ deviceId: it.deviceId }}
                    />
                  </TableCell>
                </TableRow>
              ))
            }
          </TableBody>
        </Table>
      </CardContent>
    </Card>
  )
}
