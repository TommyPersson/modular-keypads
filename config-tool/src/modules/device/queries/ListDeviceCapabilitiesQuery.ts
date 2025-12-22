import { type UseQueryOptions } from "@tanstack/react-query"
import type { DeviceCapability, DeviceFacade } from "../facade/DeviceFacade"

export const ListDeviceCapabilitiesQuery: (deviceId: string, deviceFacade: DeviceFacade) => UseQueryOptions<DeviceCapability[]> = (deviceId, deviceFacade) => ({
  queryKey: ["device-capabilities", deviceId],
  queryFn: async (): Promise<DeviceCapability[]> => {
    return deviceFacade.listDeviceCapabilities(deviceId)
  },
  enabled: deviceFacade.isConnected,
})