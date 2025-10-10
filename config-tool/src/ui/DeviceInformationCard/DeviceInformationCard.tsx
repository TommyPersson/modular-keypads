import RefreshOutlinedIcon from "@mui/icons-material/RefreshOutlined"
import SettingsOutlinedIcon from "@mui/icons-material/SettingsOutlined"
import {
  Button,
  Card,
  CardContent,
  CardHeader,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  IconButton,
  MenuItem,
  Select,
  FormControl,
  Stack,
  TextField,
  Alert,
  InputLabel,
  type SelectChangeEvent
} from "@mui/material"
import { useMutation, useQuery } from "@tanstack/react-query"
import { type ChangeEvent, useCallback, useEffect, useState } from "react"
import { PropertyGroup, PropertyText } from "../../components"
import { useDeviceContext } from "../../context/DeviceContext"
import type { DeviceFacade } from "../../facade/DeviceFacade"
import { queryClient } from "../../utils/queryClient"

export const DeviceInformationCard = () => {
  const { facade: deviceFacade, isConnected } = useDeviceContext()

  const [isDialogOpen, setIsDialogOpen] = useState(false)

  const deviceIdQuery = useQuery({
    queryKey: ["device-information"],
    queryFn: async (): Promise<DeviceInformation> => {
      return {
        deviceId: await deviceFacade.getDeviceId(),
        deviceFirmwareVersion: await deviceFacade.getDeviceFirmwareVersion(),
        deviceType: await deviceFacade.getDeviceType(),
        deviceAddress: await deviceFacade.getDeviceAddress(),
      }
    },
    enabled: isConnected,
  })

  const handleOpenDialogClicked = useCallback(() => {
    setIsDialogOpen(true)
  }, [setIsDialogOpen])

  const handleDialogClosed = useCallback(() => {
    setIsDialogOpen(false)
  }, [setIsDialogOpen])

  const deviceInformation = deviceIdQuery.data ?? null

  const canOpenDialog = !isDialogOpen && deviceInformation !== null

  return (
    <Card>
      <CardHeader
        title={"Device Information"}
        subheader={"Last read at 13:37:00"}
        action={
          <>
            <IconButton
              onClick={() => deviceIdQuery.refetch()}
              children={<RefreshOutlinedIcon />}
            />
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
            title={"ID"}
            subtitle={<code>{deviceInformation?.deviceId ?? "N/A"}</code>}
          />
          <PropertyText
            title={"Firmware Version"}
            subtitle={<code>{deviceInformation?.deviceFirmwareVersion ?? "N/A"}</code>}
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

type DeviceInformation = {
  deviceId: string
  deviceFirmwareVersion: string
  deviceType: string
  deviceAddress: number
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

  useEffect(() => {
    setDeviceTypeCode(deviceInformation.deviceType)
    setDeviceAddress(deviceInformation.deviceAddress)
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

  const saveMutation = useMutation({
    mutationFn: async (args: {deviceTypeCode: string, deviceAddress: number}) => {
      await deviceFacade.setDeviceType(args.deviceTypeCode)
      await deviceFacade.setDeviceAddress(args.deviceAddress)
      await deviceFacade.resetDevice()
    },
    onSuccess: async () => {
      await queryClient.invalidateQueries({ queryKey: ["device-information"] })
    }
  })

  const handleSaveClicked = useCallback(async () => {
    await saveMutation.mutateAsync({ deviceTypeCode, deviceAddress })
    onClose()
  }, [deviceTypeCode, deviceAddress, saveMutation, onClose])

  const isDirty = deviceTypeCode !== deviceInformation.deviceType
    || deviceAddress !== deviceInformation.deviceAddress

  const canSave = isDirty && !saveMutation.isPending;

  return (
    <Dialog open={isOpen} onClose={onClose}>
      <DialogTitle>Update Device Configuration</DialogTitle>
      <DialogContent>
        <Stack spacing={2} paddingTop={2}>
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