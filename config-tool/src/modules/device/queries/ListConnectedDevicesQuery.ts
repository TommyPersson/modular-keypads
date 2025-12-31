import {
  ListConnectedDevicesDeviceCommand
} from "@src/modules/device/facade/device-commands/ListConnectedDevicesDeviceCommand"
import { type UseQueryOptions } from "@tanstack/react-query"
import type { DeviceFacade } from "../facade/DeviceFacade"
import type { DeviceInformation } from "../models"

export const ListConnectedDevicesQuery: (deviceFacade: DeviceFacade) => UseQueryOptions<DeviceInformation[]> = (deviceFacade) => ({
  queryKey: ["connected-devices"],
  queryFn: async (): Promise<DeviceInformation[]> => {
    return deviceFacade.executeCommand(new ListConnectedDevicesDeviceCommand())
  },
  enabled: deviceFacade.isConnected,
})