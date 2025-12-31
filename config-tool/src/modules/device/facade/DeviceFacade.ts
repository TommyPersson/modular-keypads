import type { DeviceCommand } from "@src/modules/device/facade/commands/DeviceCommand"
import type { KeyBinding, KeyBindingTrigger, MacroDefinition } from "@src/modules/key-bindings/models"
import type { DateTime } from "luxon"
import { Observable } from "rxjs"
import type { DeviceCapability, DeviceInformation, DeviceRegisterValues } from "../models"

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

  listKeyBindings(): Promise<KeyBinding[]>
  clearKeyBinding(trigger: KeyBindingTrigger): Promise<void>
  setKeyBinding(trigger: KeyBindingTrigger, macroId: number): Promise<void>

  resetDevice(): Promise<void>

  executeCommand<TResult>(command: DeviceCommand<TResult>): Promise<TResult>

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