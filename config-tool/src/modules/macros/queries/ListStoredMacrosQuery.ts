import type { DeviceFacade } from "@src/modules/device/facade"
import { ListStoredMacrosDeviceCommand } from "@src/modules/device/facade/device-commands/ListStoredMacrosDeviceCommand"
import type { MacroDefinition } from "@src/modules/macros/models"
import { type UseQueryOptions } from "@tanstack/react-query"

export const ListStoredMacrosQueryKey = ["device", "macros", "stored-macros"]

export const ListStoredMacrosQuery: (deviceFacade: DeviceFacade) => UseQueryOptions<MacroDefinition[]> = (deviceFacade) => ({
  queryKey: ListStoredMacrosQueryKey,
  queryFn: async (): Promise<MacroDefinition[]> => {
    return deviceFacade.executeCommand(new ListStoredMacrosDeviceCommand())
  },
  enabled: deviceFacade.isConnected,
})
