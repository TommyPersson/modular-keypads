import { useDeviceFacade } from "@src/modules/device/context"
import type { NotificationMessage } from "@src/modules/device/models"
import { useCallback, useEffect, useState } from "react"

export type PushButtonStates = { [index: number]: boolean }

export function usePushButtonStates(deviceId: string): PushButtonStates {
  const deviceFacade = useDeviceFacade()

  const [pushButtonStates, setPushButtonStates] = useState({})

  const handleNotificationMessage = useCallback((message: NotificationMessage) => {
    if (message.deviceId !== deviceId || (message.type !== "switch.pressed" && message.type !== "switch.released")) {
      return
    }

    const number = parseInt(message.args[0])
    setPushButtonStates(s => ({ ...s, [number]: message.type === "switch.pressed" }))
  }, [deviceId, setPushButtonStates])

  useEffect(() => {
    const subscription = deviceFacade.notifications$.subscribe(handleNotificationMessage)
    return () => subscription.unsubscribe()
    // eslint-disable-next-line
  }, [handleNotificationMessage])

  useEffect(() => {

  }, [])

  return pushButtonStates
}