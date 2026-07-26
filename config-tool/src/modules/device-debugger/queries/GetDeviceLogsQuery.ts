import type { DeviceFacade } from "@src/modules/device/facade"
import type { LogMessage } from "@src/modules/device/models"
import type { UseQueryOptions } from "@tanstack/react-query"

export type HasKey = { queryKey: string[] }

export const GetDeviceLogsQuery: HasKey & ((deviceFacade: DeviceFacade) => UseQueryOptions<LogMessage[]>) = (deviceFacade) => ({
  queryKey: GetDeviceLogsQuery.queryKey,
  queryFn: async (): Promise<LogMessage[]> => {
    return deviceFacade.logs as LogMessage[]
  },
  enabled: deviceFacade.isConnected,
})

GetDeviceLogsQuery.queryKey = ["device-debugger", "device-logs"]