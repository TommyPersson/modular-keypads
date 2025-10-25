import type { DateTime } from "luxon"
import { Observable } from "rxjs"

export type DeviceInformation = {
  readonly deviceId: string
  readonly deviceFirmwareVersion: string
  readonly deviceType: string
  readonly deviceAddress: number
  readonly deviceName: string
  readonly deviceRegisterNames: string[]
}

export type DeviceRegisterValues = { [index: string]: number }

export interface DeviceFacade {
  connect(): Promise<void>
  disconnect(): Promise<void>

  getDeviceInformation(): Promise<DeviceInformation>
  getDeviceRegisterValues(): Promise<DeviceRegisterValues>

  setDeviceType(type: string): Promise<void>
  setDeviceAddress(address: number): Promise<void>
  setDeviceName(name: string): Promise<void>

  resetDevice(): Promise<void>

  performPing(): Promise<string>

  logs$: Observable<LogMessage>
  notifications$: Observable<NotificationMessage>

  isConnected$: Observable<boolean>
  isConnected: boolean
}

export type LogMessage = {
  direction: "to-device" | "to-host"
  message: string
  timestamp: DateTime
}

export type NotificationMessage = {
  type: string
  args: string[]
  timestamp: DateTime
}