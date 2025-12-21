import type { DeviceFacade } from "@src/modules/device/facade"
import type { UseQueryOptions } from "@tanstack/react-query"

export const GetTestModeQuery: (deviceFacade: DeviceFacade) => UseQueryOptions<boolean> = (deviceFacade) => ({
  queryKey: ["test-mode-state"],
  queryFn: async (): Promise<boolean> => {
    return deviceFacade.getTestMode()
  },
  enabled: deviceFacade.isConnected,
})