import { ReadMetricsDeviceCommand } from "@src/modules/device/facade/commands/ReadMetricsDeviceCommand"
import { type UseQueryOptions } from "@tanstack/react-query"
import type { DeviceFacade } from "../facade/DeviceFacade"
import type { DeviceMetric } from "../models"

export const ListDeviceMetricsQuery: (deviceFacade: DeviceFacade) => UseQueryOptions<DeviceMetric[]> = (deviceFacade) => ({
  queryKey: ["device", "metrics"],
  queryFn: async (): Promise<DeviceMetric[]> => {
    return deviceFacade.executeCommand(new ReadMetricsDeviceCommand())
  },
  enabled: deviceFacade.isConnected,
  refetchInterval: 5000,
  refetchOnMount: true,
})