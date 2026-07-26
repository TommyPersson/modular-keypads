import type { DeviceCommand } from "@src/modules/device/facade/device-commands/DeviceCommand"
import { Observable } from "rxjs"
import type { DeviceInformation, NotificationMessage, DeviceMessage, LogMessage } from "../models"

export interface DeviceFacade {
  connect(): Promise<void>
  disconnect(): Promise<void>

  getDeviceInformation(): Promise<DeviceInformation>

  executeCommand<TResult>(command: DeviceCommand<TResult>): Promise<TResult>

  messages$: Observable<DeviceMessage>
  logs$: Observable<LogMessage[]>
  notifications$: Observable<NotificationMessage>

  logs: ReadonlyArray<LogMessage>

  clearLogs(): void
  deleteLogMessage(key: number): void

  isConnected$: Observable<boolean>
  isConnected: boolean
}

