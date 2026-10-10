import { Dialog, DialogActions, DialogContent, DialogTitle, Stack, TextField } from "@mui/material"
import { CommandButton } from "@src/modules/common/components"
import { RenameDeviceCommand } from "@src/modules/device/commands/RenameDeviceCommand"
import { type ChangeEvent, useCallback, useEffect, useState } from "react"

export const RenameDeviceDialog = (props: {
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
