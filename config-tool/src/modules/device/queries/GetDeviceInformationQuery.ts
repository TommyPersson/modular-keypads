import { type UseQueryOptions } from "@tanstack/react-query"
import type { DeviceFacade } from "../facade/DeviceFacade"
import type { DeviceInformation } from "../models"

export const GetDeviceInformationQuery: (deviceFacade: DeviceFacade) => UseQueryOptions<DeviceInformation> = (deviceFacade) => ({
  queryKey: ["device-information"],
  queryFn: async (): Promise<DeviceInformation> => {
    return deviceFacade.getDeviceInformation()
  },
  enabled: deviceFacade.isConnected,
})