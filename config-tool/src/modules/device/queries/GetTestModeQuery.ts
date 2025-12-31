import type { DeviceFacade } from "@src/modules/device/facade"
import { GetTestModeDeviceCommand } from "@src/modules/device/facade/device-commands/GetTestModeDeviceCommand"
import type { UseQueryOptions } from "@tanstack/react-query"

export const GetTestModeQuery: (deviceFacade: DeviceFacade) => UseQueryOptions<boolean> = (deviceFacade) => ({
  queryKey: ["test-mode-state"],
  queryFn: async (): Promise<boolean> => {
    return deviceFacade.executeCommand(new GetTestModeDeviceCommand())
  },
  enabled: deviceFacade.isConnected,
})