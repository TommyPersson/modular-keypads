import { createContext, useContext } from "react"
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
  return useContext(DeviceContext)
}

export function useDeviceFacade(): DeviceFacade {
  const { facade } = useContext(DeviceContext)
  return facade
}