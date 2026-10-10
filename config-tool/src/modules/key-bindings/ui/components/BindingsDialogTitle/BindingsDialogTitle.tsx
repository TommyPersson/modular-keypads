import { DialogTitle, Stack, Typography } from "@mui/material"
import { CommandButton } from "@src/modules/common/components"
import {
  FlashButtonIdentificationLightCommand
} from "@src/modules/device/commands/FlashButtonIdentificationLightCommand"
import { useDeviceFacade } from "@src/modules/device/context"
import { useConnectedDevices } from "@src/modules/device/hooks/useConnectedDevices"
import { ListDeviceCapabilitiesQuery } from "@src/modules/device/queries/ListDeviceCapabilitiesQuery"
import { useQuery } from "@tanstack/react-query"

/**
 * The title of the binding dialogs, with the name of the device below it, and a button in the top-right corner for
 * flashing the identification light of the push button, to find it on the device. The button is disabled for push
 * buttons without a light.
 */
export const BindingsDialogTitle = (props: {
  title: string
  deviceId: string
  buttonNumber: number
}) => {
  const { title, deviceId, buttonNumber } = props

  const deviceFacade = useDeviceFacade()

  const connectedDevicesQuery = useConnectedDevices()
  const device = connectedDevicesQuery.data?.find(it => it.deviceId === deviceId)

  const deviceCapabilitiesQuery = useQuery(ListDeviceCapabilitiesQuery(deviceId, deviceFacade))
  const pushButton = deviceCapabilitiesQuery.data?.find(it => it.type === "PushButton" && it.number === buttonNumber)
  const hasLed = pushButton?.type === "PushButton" && pushButton.hasLed

  return (
    <DialogTitle>
      <Stack direction={"row"} justifyContent={"space-between"} alignItems={"flex-start"}>
        <span>
          {title}
          {/* A span, as the title is a heading, which can't contain paragraphs */}
          {device && (
            <Typography variant={"body2"} component={"span"} display={"block"} color={"text.secondary"}>
              {device.deviceName}
            </Typography>
          )}
        </span>
        <CommandButton
          command={FlashButtonIdentificationLightCommand}
          args={{ deviceId, buttonNumber }}
          disabled={!hasLed}
          tooltip={pushButton && !hasLed ? "This button does not have an LED indicator" : undefined}
          iconOnly
        />
      </Stack>
    </DialogTitle>
  )
}
