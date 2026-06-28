import { useDeviceFacade } from "@src/modules/device/context"
import { useMemo } from "react"
import type { MacroDefinition } from "../models"
import { ListStoredMacrosQuery } from "../queries"
import { useQuery, type UseQueryResult } from "@tanstack/react-query"

// TODO keep these simple hooks in the same file as the query?

export function useStoredMacrosQuery(): UseQueryResult<MacroDefinition[], Error> {
  const deviceFacade = useDeviceFacade()
  return useQuery(ListStoredMacrosQuery(deviceFacade))
}

export type UseStoredMacrosResult = {
  query: UseQueryResult<MacroDefinition[], Error>
  allMacros: MacroDefinition[]
  macrosByDirectoryMap: { [directory: string]: MacroDefinition[] }
  macrosByDirectoryList: [string, MacroDefinition[]][]
  macrosById: { [id: number]: MacroDefinition }
}

const EmptyArray: any[] = []

export function useStoredMacros(): UseStoredMacrosResult {
  const query = useStoredMacrosQuery()

  const unsortedMacros = query.data ?? EmptyArray

  const allMacros = useMemo(() => {
    return unsortedMacros.toSorted((a, b) => a.name.localeCompare(b.name))
  }, [unsortedMacros])

  const macrosByDirectoryMap = useMemo(() => {
    const macrosByDirectory: { [directory: string]: MacroDefinition[] } = allMacros.reduce((acc, curr: MacroDefinition) => {
      let directory = curr.directory
      if (directory === null || directory.length === 0) {
        directory = "<root>"
      }
      return ({
        ...acc,
        [directory]: [...(acc[directory] ?? []), curr].toSorted((a, b) => a.name.localeCompare(b.name))
      })
    }, {} as { [directory: string]: MacroDefinition[] })

    return macrosByDirectory
  }, [allMacros])

  const macrosByDirectoryList = useMemo(() => {
    return Object.entries(macrosByDirectoryMap).toSorted((a, b) => a[0].localeCompare(b[0]))
  }, [macrosByDirectoryMap])

  const macrosById: { [id: number]: MacroDefinition } = useMemo(() => {
    return allMacros.reduce((acc, curr) => ({ ...acc, [curr.id]: curr }), {})
  }, [allMacros])


  return {
    query,
    allMacros,
    macrosByDirectoryMap,
    macrosByDirectoryList,
    macrosById,
  }
}