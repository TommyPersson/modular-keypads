import { type UseQueryOptions } from "@tanstack/react-query"
import type { DeviceFacade, DeviceRegisterValues } from "../facade/DeviceFacade"

export const GetDeviceRegisterValuesQuery: (deviceFacade: DeviceFacade) => UseQueryOptions<DeviceRegisterValues> = (deviceFacade) => ({
  queryKey: ["device-register-values"],
  queryFn: async (): Promise<DeviceRegisterValues> => {
    return deviceFacade.getDeviceRegisterValues()
  },
  enabled: deviceFacade.isConnected,
})