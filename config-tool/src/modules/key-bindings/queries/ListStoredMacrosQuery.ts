import type { DeviceFacade } from "@src/modules/device/facade"
import { type MacroDefinition } from "@src/modules/key-bindings/models"
import { type UseQueryOptions } from "@tanstack/react-query"

export const ListStoredMacrosQueryKey = ["device", "macros", "stored-macros"]

export const ListStoredMacrosQuery: (deviceFacade: DeviceFacade) => UseQueryOptions<MacroDefinition[]> = (deviceFacade) => ({
  queryKey: ListStoredMacrosQueryKey,
  queryFn: async (): Promise<MacroDefinition[]> => {
    return deviceFacade.getStoredMacros()
  },
  enabled: deviceFacade.isConnected,
})
