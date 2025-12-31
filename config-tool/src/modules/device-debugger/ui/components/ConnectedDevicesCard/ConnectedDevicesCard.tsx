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
import { PropertyGroup, PropertyText } from "@src/modules/common/components"
import { useDeviceContext } from "@src/modules/device/context"
import type { DeviceFacade } from "@src/modules/device/facade"
import type { DeviceInformation } from "@src/modules/device/models"
import { ListConnectedDevicesQuery } from "@src/modules/device/queries/ListConnectedDevicesQuery"
import { ListDeviceCapabilitiesQuery } from "@src/modules/device/queries/ListDeviceCapabilitiesQuery"
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

      <CardContent style={{ paddingLeft: 0, paddingRight: 0 }}>
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
                <DeviceRow key={it.deviceId} device={it} deviceFacade={deviceFacade} />
              ))
            }
          </TableBody>
        </Table>
      </CardContent>
    </Card>
  )
}

const DeviceRow = (props: { device: DeviceInformation, deviceFacade: DeviceFacade }) => {
  const { device, deviceFacade } = props

  const capabilitiesQuery = useQuery(ListDeviceCapabilitiesQuery(device.deviceId, deviceFacade))
  const capabilities = capabilitiesQuery.data ?? []

  const pushButtons = capabilities.filter(it => it.type === "PushButton")
  const rotaryEncoders = capabilities.filter(it => it.type === "RotaryEncoder")

  return (
    <>
      <TableRow>
        <TableCell><code>{device.deviceId}</code></TableCell>
        <TableCell><code>{device.deviceAddress}</code></TableCell>
        <TableCell>{device.deviceName}</TableCell>
        <TableCell><code>{device.deviceType}</code></TableCell>
      </TableRow>
      <TableRow>
        <TableCell></TableCell>
        <TableCell colSpan={3}>
          <PropertyGroup horizontal>
            <PropertyText title={"# Push Buttons"} subtitle={pushButtons.length} />
            <PropertyText title={"# Rotary Encoders"} subtitle={rotaryEncoders.length} />
          </PropertyGroup>
        </TableCell>
      </TableRow>
    </>
  )
}