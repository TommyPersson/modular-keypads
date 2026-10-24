import InfoOutlinedIcon from "@mui/icons-material/InfoOutlined"
import SystemUpdateAltOutlinedIcon from "@mui/icons-material/SystemUpdateAltOutlined"
import { IconButton, Tooltip } from "@mui/material"
import { CommandButton } from "@src/modules/common/components"
import {
  FlashDeviceIdentificationLightsCommand
} from "@src/modules/device/commands/FlashDeviceIdentificationLightsCommand"
import { RenameDeviceCommand } from "@src/modules/device/commands/RenameDeviceCommand"
import { globalFirmwareUpdateController } from "@src/modules/device/firmware"
import type { DeviceInformation } from "@src/modules/device/models"
import { useCallback, useState } from "react"
import { RenameDeviceDialog } from "./RenameDeviceDialog"

/**
 * Sizes of the device actions: small in card headers, and compact where only a caption's height is available.
 */
export type DeviceActionSize = "small" | "compact"

const compactButtonSx = { p: "1px", "& svg": { fontSize: 14 } }

/**
 * An info icon, with the details of the device in its tooltip.
 */
export const DeviceInfoIcon = (props: {
  device: DeviceInformation
  size: DeviceActionSize
}) => {
  const { device, size } = props

  return (
    <Tooltip
      title={
        <>
          <div>Type: <code>{device.deviceType}</code></div>
          <div>ID: <code>{device.deviceId}</code></div>
          <div>Address: <code>{device.deviceAddress}</code></div>
        </>
      }
    >
      {size === "compact"
        ? <InfoOutlinedIcon color={"action"} sx={{ fontSize: 14, mx: "1px" }} />
        : <InfoOutlinedIcon color={"action"} fontSize={"small"} />}
    </Tooltip>
  )
}

export const IdentifyDeviceButton = (props: {
  device: DeviceInformation
  size: DeviceActionSize
}) => {
  const { device, size } = props

  return (
    <CommandButton
      command={FlashDeviceIdentificationLightsCommand}
      args={{ deviceId: device.deviceId }}
      iconOnly
      size={"small"}
      sx={size === "compact" ? compactButtonSx : undefined}
    />
  )
}

export const RenameDeviceButton = (props: {
  device: DeviceInformation
  size: DeviceActionSize
}) => {
  const { device, size } = props

  const [isDialogOpen, setIsDialogOpen] = useState<boolean>(false)

  const handleClick = useCallback(() => {
    setIsDialogOpen(true)
  }, [])

  const handleDialogClose = useCallback(() => {
    setIsDialogOpen(false)
  }, [])

  return (
    <>
      <Tooltip title={RenameDeviceCommand.label}>
        <IconButton size={"small"} sx={size === "compact" ? compactButtonSx : undefined} onClick={handleClick}>
          {RenameDeviceCommand.icon}
        </IconButton>
      </Tooltip>
      <RenameDeviceDialog
        isOpen={isDialogOpen}
        deviceId={device.deviceId}
        deviceName={device.deviceName}
        onClose={handleDialogClose}
      />
    </>
  )
}

/**
 * Opens the firmware update dialog. Only the connected device, the master, can be updated, as the others are only
 * reachable through it.
 */
export const UpdateFirmwareButton = (props: {
  device: DeviceInformation
  size: DeviceActionSize
}) => {
  const { device, size } = props

  const handleClick = useCallback(() => {
    globalFirmwareUpdateController.open(device.deviceName)
  }, [device.deviceName])

  return (
    <Tooltip title={"Update firmware"}>
      <IconButton size={"small"} sx={size === "compact" ? compactButtonSx : undefined} onClick={handleClick}>
        <SystemUpdateAltOutlinedIcon />
      </IconButton>
    </Tooltip>
  )
}
