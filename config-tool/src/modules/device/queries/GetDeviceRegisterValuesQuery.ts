import type { DeviceRegisterValues } from "../models"
import { type UseQueryOptions } from "@tanstack/react-query"
import type { DeviceFacade } from "../facade/DeviceFacade"

export const GetDeviceRegisterValuesQuery: (deviceFacade: DeviceFacade) => UseQueryOptions<DeviceRegisterValues> = (deviceFacade) => ({
  queryKey: ["device-register-values"],
  queryFn: async (): Promise<DeviceRegisterValues> => {
    return deviceFacade.getDeviceRegisterValues()
  },
  enabled: deviceFacade.isConnected,
})