import type { DateTime } from "luxon"
import { Observable } from "rxjs"

export interface DeviceFacade {
  connect(): Promise<void>
  disconnect(): Promise<void>

  getDeviceId(): Promise<string>
  getDeviceFirmwareVersion(): Promise<string>
  getDeviceType(): Promise<string>
  getDeviceAddress(): Promise<number>

  setDeviceType(type: string): Promise<void>
  setDeviceAddress(address: number): Promise<void>

  resetDevice(): Promise<void>

  performPing(): Promise<string>

  $logs: Observable<LogMessage>
  $isConnected: Observable<boolean>
}

export type LogMessage = {
  direction: "to-device" | "to-host"
  message: string
  timestamp: DateTime
}


