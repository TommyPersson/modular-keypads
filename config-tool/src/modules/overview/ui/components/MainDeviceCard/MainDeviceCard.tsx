import SettingsOutlinedIcon from "@mui/icons-material/SettingsOutlined"

import {
  Alert,
  Button,
  Card,
  CardContent,
  CardHeader,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  FormControl,
  IconButton,
  InputLabel,
  MenuItem,
  Select,
  type SelectChangeEvent,
  Stack,
  TextField
} from "@mui/material"
import { PropertyGroup, PropertyText } from "@src/modules/common/components"
import { useDeviceContext } from "@src/modules/device/context"
import type { DeviceFacade } from "@src/modules/device/facade"
import type { DeviceInformation } from "@src/modules/device/models"
import { GetDeviceInformationQuery } from "@src/modules/device/queries"
import { queryClient } from "@src/utils/queryClient"
import { useMutation, useQuery } from "@tanstack/react-query"
import { type ChangeEvent, useCallback, useEffect, useState } from "react"

export const MainDeviceCard = () => {
  const { facade: deviceFacade } = useDeviceContext()

  const [isDialogOpen, setIsDialogOpen] = useState(false)

  const deviceInformationQuery = useQuery(GetDeviceInformationQuery(deviceFacade))

  const handleOpenDialogClicked = useCallback(() => {
    setIsDialogOpen(true)
  }, [setIsDialogOpen])

  const handleDialogClosed = useCallback(() => {
    setIsDialogOpen(false)
  }, [setIsDialogOpen])

  const deviceInformation = deviceInformationQuery.data ?? null

  const canOpenDialog = !isDialogOpen && deviceInformation !== null

  return (
    <Card>
      <CardHeader
        title={"Main Device"}
        action={
          <>
            <IconButton
              disabled={!canOpenDialog}
              onClick={handleOpenDialogClicked}
              children={<SettingsOutlinedIcon />}
            />
          </>
        }
      />

      <CardContent>
        <PropertyGroup>
          <PropertyText
            title={"Name"}
            subtitle={<code>{deviceInformation?.deviceName ?? "N/A"}</code>}
          />
          <PropertyText
            title={"ID"}
            subtitle={<code>{deviceInformation?.deviceId ?? "N/A"}</code>}
          />
          <PropertyText
            title={"Type"}
            subtitle={<code>{deviceInformation?.deviceType ?? "N/A"}</code>}
          />
          <PropertyText
            title={"Address"}
            subtitle={<code>{deviceInformation?.deviceAddress ?? "N/A"}</code>}
          />
        </PropertyGroup>
      </CardContent>
      {deviceInformation && (
        <UpdateDeviceConfigurationDialog
          isOpen={isDialogOpen}
          onClose={handleDialogClosed}
          deviceInformation={deviceInformation}
          deviceFacade={deviceFacade}
        />
      )}
    </Card>
  )
}

type DeviceType = {
  displayName: string
  code: string
}

const deviceTypes: DeviceType[] = [
  {
    code: "0",
    displayName: "Generic"
  },
  {
    code: "m",
    displayName: "Master"
  },
  {
    code: "a",
    displayName: "A"
  },
]

const UpdateDeviceConfigurationDialog = (props: {
  isOpen: boolean,
  onClose: () => void
  deviceInformation: DeviceInformation,
  deviceFacade: DeviceFacade
}) => {

  const {
    isOpen,
    onClose,
    deviceInformation,
    deviceFacade,
  } = props

  const [deviceTypeCode, setDeviceTypeCode] = useState(deviceInformation.deviceType)
  const [deviceAddress, setDeviceAddress] = useState(deviceInformation.deviceAddress)
  const [deviceName, setDeviceName] = useState(deviceInformation.deviceName)

  useEffect(() => {
    setDeviceTypeCode(deviceInformation.deviceType)
    setDeviceAddress(deviceInformation.deviceAddress)
    setDeviceName(deviceInformation.deviceName)
  }, [deviceInformation, isOpen])

  const handleDeviceTypeCodeChanged = useCallback((event: SelectChangeEvent) => {
    setDeviceTypeCode(event.target.value)
  }, [setDeviceTypeCode])

  const handleDeviceAddressChanged = useCallback((event: ChangeEvent<HTMLInputElement>) => {
    let newValue = parseInt(event.target.value)
    if (isNaN(newValue)) {
      newValue = 0
    }

    setDeviceAddress(newValue)
  }, [setDeviceAddress])

  const handleDeviceNameChanged = useCallback((event: ChangeEvent<HTMLInputElement>) => {
    setDeviceName(event.target.value)
  }, [setDeviceName])

  const saveMutation = useMutation({
    mutationFn: async (args: {deviceTypeCode: string, deviceAddress: number, deviceName: string}) => {
      await deviceFacade.setDeviceType(args.deviceTypeCode)
      await deviceFacade.setDeviceAddress(args.deviceAddress)
      await deviceFacade.setDeviceName(args.deviceName)
      await deviceFacade.resetDevice()
    },
    onSuccess: async () => {
      await queryClient.invalidateQueries({ queryKey: ["device-information"] })
    }
  })

  const handleSaveClicked = useCallback(async () => {
    await saveMutation.mutateAsync({ deviceTypeCode, deviceAddress, deviceName })
    onClose()
  }, [deviceTypeCode, deviceAddress, deviceName, saveMutation, onClose])

  const isDirty = deviceTypeCode !== deviceInformation.deviceType
    || deviceAddress !== deviceInformation.deviceAddress
    || deviceName !== deviceInformation.deviceName

  const canSave = isDirty && !saveMutation.isPending;

  return (
    <Dialog open={isOpen} onClose={onClose}>
      <DialogTitle>Update Device Configuration</DialogTitle>
      <DialogContent>
        <Stack spacing={2} paddingTop={2}>
          <TextField
            label={"Device Name"}
            value={deviceName}
            onChange={handleDeviceNameChanged}
            helperText={"At most 30 characters"}
          />
          <FormControl>
            <InputLabel>Device Type</InputLabel>
            <Select
              value={deviceTypeCode}
              label={"Device Type"}
              onChange={handleDeviceTypeCodeChanged}
            >
              {deviceTypes.map(it => (
                <MenuItem
                  key={it.code}
                  value={it.code}
                  children={it.displayName}
                />
              ))}
            </Select>
          </FormControl>
          <TextField
            label={"Device Address"}
            value={deviceAddress}
            onChange={handleDeviceAddressChanged}
            helperText={"1-127"}
          />
          <Alert severity={"info"}>
            The device will be reset upon saving. You may need to reconnect.
          </Alert>
        </Stack>

      </DialogContent>
      <DialogActions>
        <Button
          disabled={!canSave}
          loading={saveMutation.isPending}
          onClick={handleSaveClicked}
          children={"Save"}
        />
      </DialogActions>
    </Dialog>
  )

}