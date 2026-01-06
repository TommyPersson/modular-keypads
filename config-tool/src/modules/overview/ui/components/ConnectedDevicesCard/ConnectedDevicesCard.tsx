import {
  Alert,
  Card,
  CardContent,
  CardHeader, Dialog, DialogActions, DialogContent, DialogTitle,
  IconButton, Stack,
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableRow, TextField, Tooltip
} from "@mui/material"
import { CommandButton } from "@src/modules/common/components"
import {
  FlashDeviceIdentificationLightsCommand
} from "@src/modules/device/commands/FlashDeviceIdentificationLightsCommand"
import { RenameDeviceCommand } from "@src/modules/device/commands/RenameDeviceCommand"
import { useDeviceContext } from "@src/modules/device/context"
import type { DeviceInformation } from "@src/modules/device/models"
import { ListConnectedDevicesQuery } from "@src/modules/device/queries/ListConnectedDevicesQuery"
import { useQuery } from "@tanstack/react-query"
import { type ChangeEvent, useCallback, useEffect, useState } from "react"

export const ConnectedDevicesCard = () => {
  const { facade: deviceFacade } = useDeviceContext()

  const query = useQuery(ListConnectedDevicesQuery(deviceFacade))

  const connectedDevices = query.data ?? []

  return (
    <Card>
      <CardHeader title={"Connected Devices"} />
      <CardContent style={{ paddingLeft: 0, paddingRight: 0 }}>
        <Table size={"small"}>
          <TableHead>
            <TableRow>
              <TableCell>Name</TableCell>
              <TableCell style={{ width: 0 }}>ID</TableCell>
              <TableCell>Type</TableCell>
              <TableCell>Address</TableCell>
              <TableCell style={{ width: 0 }} align={"right"}></TableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            {
              connectedDevices.map(it => (
                <DeviceRow key={it.deviceId} device={it} />
              ))
            }
          </TableBody>
        </Table>
      </CardContent>
    </Card>
  )
}

const DeviceRow = (props: { device: DeviceInformation }) => {
  const { device } = props

  const [isRenameDialogOpen, setIsRenameDialogOpen] = useState<boolean>(false)

  const handleRenameDeviceClicked = useCallback(() => {
    setIsRenameDialogOpen(true)
  }, [setIsRenameDialogOpen])

  const handleRenameDeviceDialogClosed = useCallback(() => {
    setIsRenameDialogOpen(false)
  }, [setIsRenameDialogOpen])

  return (
    <>
      <TableRow>
        <TableCell>{device.deviceName}</TableCell>
        <TableCell><code>{device.deviceId}</code></TableCell>
        <TableCell><code>{device.deviceType}</code></TableCell>
        <TableCell><code>{device.deviceAddress}</code></TableCell>
        <TableCell>
          <Stack direction={"row"}>
            <CommandButton
              command={FlashDeviceIdentificationLightsCommand}
              args={{ deviceId: device.deviceId }}
              iconOnly
            />
            <Tooltip title={RenameDeviceCommand.label}>
              <IconButton onClick={handleRenameDeviceClicked}>
                {RenameDeviceCommand.icon}
              </IconButton>
            </Tooltip>
          </Stack>
        </TableCell>
      </TableRow>
      <RenameDeviceDialog
        isOpen={isRenameDialogOpen}
        deviceId={device.deviceId}
        deviceName={device.deviceName}
        onClose={handleRenameDeviceDialogClosed}
      />
    </>
  )

}

const RenameDeviceDialog = (props: {
  isOpen: boolean
  deviceId: string
  deviceName: string
  onClose: () => void
}) => {
  const { isOpen, deviceId, deviceName, onClose } = props

  const [newDeviceName, setNewDeviceName] = useState<string>(deviceName)

  const handleChange = useCallback((event: ChangeEvent<HTMLInputElement>) => {
    setNewDeviceName(event.target.value ?? "")
  }, [setNewDeviceName])

  useEffect(() => {
    setNewDeviceName(deviceName)
  }, [isOpen, deviceName, setNewDeviceName])

  return (
    <Dialog open={isOpen} onClose={onClose}>
      <DialogTitle>Rename Device</DialogTitle>
      <DialogContent>
        <Stack spacing={2} sx={{ mt: 2 }}>
          <TextField
            label={"Name"}
            value={newDeviceName}
            onChange={handleChange}
          />
          <Alert severity={"info"}>
            The devices needs to be restarted for the new names to take effect.
          </Alert>
        </Stack>
      </DialogContent>
      <DialogActions>
        <CommandButton
          command={RenameDeviceCommand}
          args={{ deviceId, deviceName: newDeviceName }}
          variant={"contained"}
          onSuccess={onClose}
        />
      </DialogActions>
    </Dialog>
  )
}