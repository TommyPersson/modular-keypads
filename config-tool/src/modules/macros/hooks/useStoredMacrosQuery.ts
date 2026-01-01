import { useDeviceFacade } from "@src/modules/device/context"
import type { MacroDefinition } from "../models"
import { ListStoredMacrosQuery } from "../queries"
import { useQuery, type UseQueryResult } from "@tanstack/react-query"

// TODO keep these simple hooks in the same file as the query?

export function useStoredMacrosQuery(): UseQueryResult<MacroDefinition[], Error> {
  const deviceFacade = useDeviceFacade()
  return useQuery(ListStoredMacrosQuery(deviceFacade))
}