import { GetDeviceLogsQuery } from "@src/modules/device-debugger/queries"
import { useQueryClient } from "@tanstack/react-query"
import { createContext, useContext, useEffect } from "react"
import { type DeviceFacade } from "../facade/DeviceFacade"
import { DeviceFacadeImpl } from "../facade/DeviceFacadeImpl"

export type DeviceContextValue = {
  facade: DeviceFacade
  isConnected: boolean
}

export const defaultDeviceContextValue = {
  facade: new DeviceFacadeImpl() as DeviceFacade,
  isConnected: false
}

export const globalDeviceFacade = {
  value: defaultDeviceContextValue.facade
}

export const DeviceContext = createContext<DeviceContextValue>(defaultDeviceContextValue)

export function useDeviceContext(): DeviceContextValue {
  const context = useContext(DeviceContext)
  const queryClient = useQueryClient()

  useEffect(() => {
    const handle = context.facade.logs$.subscribe(() => {
      queryClient.invalidateQueries({ queryKey: GetDeviceLogsQuery.queryKey }).then()
    })

    return () => handle.unsubscribe()
  }, [context, queryClient])

  return context
}

export function useDeviceFacade(): DeviceFacade {
  const { facade } = useContext(DeviceContext)
  return facade
}