import {
  ListRegisterValuesDeviceCommand
} from "@src/modules/device/facade/device-commands/ListDeviceRegisterValuesDeviceCommand"
import { type UseQueryOptions } from "@tanstack/react-query"
import type { DeviceFacade } from "../facade/DeviceFacade"
import type { DeviceRegisterValue } from "../models"

export const GetDeviceRegisterValuesQuery: (deviceFacade: DeviceFacade) => UseQueryOptions<DeviceRegisterValue[]> = (deviceFacade) => ({
  queryKey: ["device", "register-values"],
  queryFn: async (): Promise<DeviceRegisterValue[]> => {
    return deviceFacade.executeCommand(new ListRegisterValuesDeviceCommand())
  },
  enabled: deviceFacade.isConnected,
})