import { useDeviceFacade } from "@src/modules/device/context"
import type { Setting } from "@src/modules/settings/models"
import { ListSettingsQuery } from "@src/modules/settings/queries"
import { useQuery, type UseQueryResult } from "@tanstack/react-query"

export function useSettingsQuery(): UseQueryResult<Setting[], Error> {
  const deviceFacade = useDeviceFacade()
  return useQuery(ListSettingsQuery(deviceFacade))
}
