import { useDeviceFacade } from "@src/modules/device/context"
import type { KeyBinding } from "@src/modules/key-bindings/models"
import { ListKeyBindingsQuery } from "@src/modules/key-bindings/queries"
import { useQuery, type UseQueryResult } from "@tanstack/react-query"

// TODO keep these simple hooks in the same file as the query?

export function useKeyBindingsQuery(deviceId: string): UseQueryResult<KeyBinding[], Error> {
  const deviceFacade = useDeviceFacade()
  return useQuery(ListKeyBindingsQuery(deviceFacade, deviceId))
}