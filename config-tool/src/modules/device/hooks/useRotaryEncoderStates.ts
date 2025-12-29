import { useDeviceFacade } from "@src/modules/device/context"
import type { NotificationMessage } from "@src/modules/device/facade"
import { useCallback, useEffect, useState } from "react"

export type RotaryEncoderState = {
  clockwise: boolean
  counterClockwise: boolean
}

export type RotaryEncoderStates = { [index: number]: RotaryEncoderState }

export function useRotaryEncoderStates(deviceId: string, decayTimeMs: number = 1000): RotaryEncoderStates {
  const deviceFacade = useDeviceFacade()

  const [state, setState] = useState<RotaryEncoderStates>({})

  const handleNotificationMessage = useCallback((message: NotificationMessage) => {
    if (message.deviceId !== deviceId || !message.type.startsWith("encoder.rotated")) {
      return
    }

    const number = parseInt(message.args[0])

    if (message.type === "encoder.rotated.clockwise") {
      setState(s => ({ ...s, [number]: { ...s[number], clockwise: true } }))
      setTimeout(() => {
        setState(s => ({ ...s, [number]: { ...s[number], clockwise: false } }))
      }, decayTimeMs)
    }

    if (message.type === "encoder.rotated.counter-clockwise") {
      setState(s => ({ ...s, [number]: { ...s[number], counterClockwise: true } }))
      setTimeout(() => {
        setState(s => ({ ...s, [number]: { ...s[number], counterClockwise: false } }))
      }, decayTimeMs)
    }

  }, [deviceId, setState, decayTimeMs])

  useEffect(() => {
    const subscription = deviceFacade.notifications$.subscribe(handleNotificationMessage)
    return () => subscription.unsubscribe()
    // eslint-disable-next-line
  }, [handleNotificationMessage])

  useEffect(() => {

  }, [])

  return state
}