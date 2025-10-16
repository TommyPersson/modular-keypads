import { type UseQueryOptions } from "@tanstack/react-query"
import type { DeviceFacade, DeviceInformation } from "../facade/DeviceFacade"

export const GetDeviceInformationQuery: (deviceFacade: DeviceFacade) => UseQueryOptions<DeviceInformation> = (deviceFacade) => ({
  queryKey: ["device-information"],
  queryFn: async (): Promise<DeviceInformation> => {
    return deviceFacade.getDeviceInformation()
  },
  enabled: deviceFacade.isConnected,
})