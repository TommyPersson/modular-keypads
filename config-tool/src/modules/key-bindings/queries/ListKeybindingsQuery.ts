import type { DeviceFacade } from "@src/modules/device/facade"
import { ListKeyBindingsDeviceCommand } from "@src/modules/device/facade/device-commands/ListKeyBindingsDeviceCommand"
import { type KeyBinding } from "@src/modules/key-bindings/models"
import { type UseQueryOptions } from "@tanstack/react-query"

export const ListKeyBindingsQueryKey = ["device", "key-bindings"]

export const ListKeyBindingsQuery: (deviceFacade: DeviceFacade) => UseQueryOptions<KeyBinding[]> = (deviceFacade) => ({
  queryKey: ListKeyBindingsQueryKey,
  queryFn: async (): Promise<KeyBinding[]> => {
    return await deviceFacade.executeCommand(new ListKeyBindingsDeviceCommand())
  },
  enabled: deviceFacade.isConnected,

})
