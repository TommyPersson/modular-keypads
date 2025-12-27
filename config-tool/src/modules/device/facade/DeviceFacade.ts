import type { MacroDefinition } from "@src/modules/key-bindings/models"
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

export type BaseCapability = {
  type: string
  index: number
}

export type PushButtonCapability = BaseCapability & {
  type: "PushButton"
  number: number
}

export type RotaryEncoderCapability = BaseCapability & {
  type: "RotaryEncoder"
  number: number
}

export type DeviceCapability = PushButtonCapability | RotaryEncoderCapability

export interface DeviceFacade {
  connect(): Promise<void>
  disconnect(): Promise<void>

  getDeviceInformation(): Promise<DeviceInformation>
  getDeviceRegisterValues(): Promise<DeviceRegisterValues>
  listConnectedDevices(): Promise<DeviceInformation[]>
  listDeviceCapabilities(deviceId: string): Promise<DeviceCapability[]>

  setDeviceType(type: string): Promise<void>
  setDeviceAddress(address: number): Promise<void>
  setDeviceName(name: string): Promise<void>

  getTestMode(): Promise<boolean>
  setTestMode(enabled: boolean): Promise<void>

  // TODO allow other modules access to low level command interface instead
  // of forcing the facade to do everything. (Though "facade" implies a wide responsibility anyway...)
  saveMacro(macro: MacroDefinition): Promise<void>
  deleteMacro(id: number): Promise<void>
  getStoredMacros(): Promise<MacroDefinition[]>

  resetDevice(): Promise<void>

  performPing(): Promise<string>

  logs$: Observable<RawLogMessage>
  notifications$: Observable<NotificationMessage>

  isConnected$: Observable<boolean>
  isConnected: boolean
}

export type RawLogMessage = {
  direction: "to-device" | "to-host"
  message: string
  timestamp: DateTime
}

export type NotificationMessage = {
  deviceId: string
  type: string
  args: string[]
  timestamp: DateTime
}