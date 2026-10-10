import type { DeviceFacade } from "@src/modules/device/facade"
import { ListSettingsDeviceCommand } from "@src/modules/device/facade/device-commands/ListSettingsDeviceCommand"
import type { Setting } from "@src/modules/settings/models"
import type { UseQueryOptions } from "@tanstack/react-query"

export const ListSettingsQueryKey = ["device", "settings"]

export const ListSettingsQuery: (deviceFacade: DeviceFacade) => UseQueryOptions<Setting[]> = (deviceFacade) => ({
  queryKey: ListSettingsQueryKey,
  queryFn: async (): Promise<Setting[]> => {
    return await deviceFacade.executeCommand(new ListSettingsDeviceCommand())
  },
  enabled: deviceFacade.isConnected,
})
