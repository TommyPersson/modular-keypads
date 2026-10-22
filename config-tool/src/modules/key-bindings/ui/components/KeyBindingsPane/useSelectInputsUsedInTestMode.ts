import { useDeviceFacade } from "@src/modules/device/context"
import { useConnectedDevices } from "@src/modules/device/hooks/useConnectedDevices"
import type { DeviceInformation, NotificationMessage } from "@src/modules/device/models"
import { GetTestModeQuery } from "@src/modules/device/queries/GetTestModeQuery"
import { useQuery } from "@tanstack/react-query"
import { useEffect, useRef } from "react"
import { type KeyBindingTarget, useKeyBindingSelection } from "./KeyBindingSelectionContext"

/**
 * Selects the input that is used on a device while it's in test mode, so that its bindings can be edited by pressing
 * it, rather than finding it in the previews. Switches are selected when they're pressed, and rotary encoders when
 * they're rotated or their button is pressed. Nothing is selected outside test mode, when using the inputs runs their
 * macros instead.
 */
export function useSelectInputsUsedInTestMode() {
  const deviceFacade = useDeviceFacade()
  const { selectedTarget, setSelectedTarget } = useKeyBindingSelection()

  const testModeQuery = useQuery(GetTestModeQuery(deviceFacade))
  const isTestMode = testModeQuery.data ?? false

  // Read when a notification arrives, rather than subscribing again whenever they change
  const devices = useConnectedDevices().data
  const latest = useRef({ devices, selectedTarget })
  useEffect(() => {
    latest.current = { devices, selectedTarget }
  }, [devices, selectedTarget])

  useEffect(() => {
    if (!isTestMode) {
      return
    }

    const subscription = deviceFacade.notifications$.subscribe(message => {
      const device = latest.current.devices?.find(it => it.deviceId === message.deviceId)
      const target = device && usedInput(message, device)
      // Not selected again while it's still used, e.g. while an encoder is rotated
      if (target && !isSameTarget(target, latest.current.selectedTarget)) {
        setSelectedTarget(target)
      }
    })
    return () => subscription.unsubscribe()
  }, [deviceFacade, isTestMode, setSelectedTarget])
}

function usedInput(message: NotificationMessage, device: DeviceInformation): KeyBindingTarget | null {
  const { deviceId } = device
  const number = parseInt(message.args[0])

  switch (message.type) {
    case "switch.pressed":
      // The button of the rotary encoder on the encoder sub-devices, which is edited with the encoder, as in the preview
      return device.deviceType === "smc3"
        ? { type: "RotaryEncoder", deviceId, encoderNumber: 1, buttonNumber: number }
        : { type: "PushButton", deviceId, buttonNumber: number }
    case "encoder.rotated.clockwise":
    case "encoder.rotated.counter-clockwise":
      // The encoder's button has the same number as the encoder
      return { type: "RotaryEncoder", deviceId, encoderNumber: number, buttonNumber: number }
    default:
      return null
  }
}

function isSameTarget(a: KeyBindingTarget, b: KeyBindingTarget | null) {
  return a.type === b?.type && a.deviceId === b.deviceId && a.buttonNumber === b.buttonNumber
}
