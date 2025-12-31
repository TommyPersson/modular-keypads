import {
  ListDeviceCapabilitiesDeviceCommand
} from "@src/modules/device/facade/device-commands/ListDeviceCapabilitiesDeviceCommand"
import { type UseQueryOptions } from "@tanstack/react-query"
import type { DeviceFacade } from "../facade/DeviceFacade"
import type { DeviceCapability } from "../models"

export const ListDeviceCapabilitiesQuery: (deviceId: string, deviceFacade: DeviceFacade) => UseQueryOptions<DeviceCapability[]> = (deviceId, deviceFacade) => ({
  queryKey: ["device", "capabilities", deviceId],
  queryFn: async (): Promise<DeviceCapability[]> => {
    return deviceFacade.executeCommand(new ListDeviceCapabilitiesDeviceCommand(deviceId))
  },
  enabled: deviceFacade.isConnected && deviceId.length > 0,
})