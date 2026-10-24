import { useEffect, useState } from "react"
import { type FirmwareUpdateState, globalFirmwareUpdateController } from "./FirmwareUpdateController"

export function useFirmwareUpdateState(): FirmwareUpdateState {
  const [state, setState] = useState(globalFirmwareUpdateController.state)

  useEffect(() => {
    const subscription = globalFirmwareUpdateController.state$.subscribe(setState)
    return () => subscription.unsubscribe()
  }, [])

  return state
}
