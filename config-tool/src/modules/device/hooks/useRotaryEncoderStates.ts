import { useDeviceFacade } from "@src/modules/device/context"
import type { NotificationMessage } from "@src/modules/device/models"
import { useCallback, useEffect, useRef, useState } from "react"

export type RotaryEncoderState = {
  clockwise: boolean
  counterClockwise: boolean
}

export type RotaryEncoderStates = { [index: number]: RotaryEncoderState }

type Direction = keyof RotaryEncoderState

const DIRECTIONS_BY_NOTIFICATION_TYPE: { [type: string]: Direction } = {
  "encoder.rotated.clockwise": "clockwise",
  "encoder.rotated.counter-clockwise": "counterClockwise",
}

export function useRotaryEncoderStates(deviceId: string, decayTimeMs: number = 1000): RotaryEncoderStates {
  const deviceFacade = useDeviceFacade()

  const [state, setState] = useState<RotaryEncoderStates>({})

  // By encoder and direction, so that continued rotation restarts the decay instead of an earlier rotation ending it
  const decayTimers = useRef(new Map<string, ReturnType<typeof setTimeout>>())

  const handleNotificationMessage = useCallback((message: NotificationMessage) => {
    const direction = DIRECTIONS_BY_NOTIFICATION_TYPE[message.type]
    if (message.deviceId !== deviceId || direction === undefined) {
      return
    }

    const number = parseInt(message.args[0])
    const timerKey = `${number}.${direction}`

    setState(s => ({ ...s, [number]: { ...s[number], [direction]: true } }))

    clearTimeout(decayTimers.current.get(timerKey))
    decayTimers.current.set(timerKey, setTimeout(() => {
      decayTimers.current.delete(timerKey)
      setState(s => ({ ...s, [number]: { ...s[number], [direction]: false } }))
    }, decayTimeMs))
  }, [deviceId, setState, decayTimeMs])

  useEffect(() => {
    const subscription = deviceFacade.notifications$.subscribe(handleNotificationMessage)
    return () => subscription.unsubscribe()
    // eslint-disable-next-line
  }, [handleNotificationMessage])

  useEffect(() => {
    const timers = decayTimers.current
    return () => {
      timers.forEach(clearTimeout)
      timers.clear()
    }
  }, [])

  return state
}
