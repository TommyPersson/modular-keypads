import { useDeviceFacade } from "@src/modules/device/context"
import { MockDeviceFacade } from "./MockDeviceFacade"

/**
 * The mock device of the surrounding mock device provider, for stories that change the device while shown.
 */
export function useMockDeviceFacade(): MockDeviceFacade {
  const facade = useDeviceFacade()
  if (!(facade instanceof MockDeviceFacade)) {
    throw new Error("Not within a mock device provider")
  }
  return facade
}
