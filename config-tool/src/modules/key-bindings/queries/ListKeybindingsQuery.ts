import type { DeviceFacade } from "@src/modules/device/facade"
import { type KeyBinding } from "@src/modules/key-bindings/models"
import { type UseQueryOptions } from "@tanstack/react-query"

export const ListKeyBindingsQueryKey = ["device", "key-bindings"]

export const ListKeyBindingsQuery: (deviceFacade: DeviceFacade) => UseQueryOptions<KeyBinding[]> = (deviceFacade) => ({
  queryKey: ListKeyBindingsQueryKey,
  queryFn: async (): Promise<KeyBinding[]> => {
    return await deviceFacade.listKeyBindings()
  },
  enabled: deviceFacade.isConnected,

})
