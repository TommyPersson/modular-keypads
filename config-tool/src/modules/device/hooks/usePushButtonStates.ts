import { useDeviceFacade } from "@src/modules/device/context"
import type { NotificationMessage } from "@src/modules/device/models"
import { useCallback, useEffect, useState } from "react"

export enum PushButtonState {
  Released = "Released",
  Pressed = "Pressed",
  // Still pressed, after being held for a while
  LongPressed = "LongPressed",
}

export type PushButtonStates = { [index: number]: PushButtonState }

const STATES_BY_NOTIFICATION_TYPE: { [type: string]: PushButtonState } = {
  "switch.pressed": PushButtonState.Pressed,
  "switch.long-pressed": PushButtonState.LongPressed,
  "switch.released": PushButtonState.Released,
}

export function isPushButtonPressed(state: PushButtonState | undefined): boolean {
  return state === PushButtonState.Pressed || state === PushButtonState.LongPressed
}

export function usePushButtonStates(deviceId: string): PushButtonStates {
  const deviceFacade = useDeviceFacade()

  const [pushButtonStates, setPushButtonStates] = useState<PushButtonStates>({})

  const handleNotificationMessage = useCallback((message: NotificationMessage) => {
    const state = STATES_BY_NOTIFICATION_TYPE[message.type]
    if (message.deviceId !== deviceId || state === undefined) {
      return
    }

    const number = parseInt(message.args[0])
    setPushButtonStates(s => ({ ...s, [number]: state }))
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