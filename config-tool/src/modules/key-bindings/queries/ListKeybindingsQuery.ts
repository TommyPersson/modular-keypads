import type { DeviceFacade } from "@src/modules/device/facade"
import { type KeyBinding } from "@src/modules/key-bindings/models"
import { type UseQueryOptions } from "@tanstack/react-query"

export const ListKeyBindingsQueryKeyPrefix = ["device", "key-bindings"]

export const ListKeyBindingsQuery: (deviceFacade: DeviceFacade, deviceId: string) => UseQueryOptions<KeyBinding[]> = (deviceFacade, deviceId) => ({
  queryKey: [...ListKeyBindingsQueryKeyPrefix, deviceId],
  queryFn: async (): Promise<KeyBinding[]> => {
    return deviceFacade.listKeyBindings(deviceId)
  },
  enabled: deviceFacade.isConnected,

})
