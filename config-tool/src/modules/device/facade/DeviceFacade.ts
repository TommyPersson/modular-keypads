import type { DeviceCommand } from "@src/modules/device/facade/device-commands/DeviceCommand"
import { Observable } from "rxjs"
import type { DeviceInformation, NotificationMessage, DeviceMessage, LogMessage } from "../models"

export interface DeviceFacade {
  connect(): Promise<void>
  disconnect(): Promise<void>

  /**
   * Reconnects to the device once it has restarted, without asking the user to select its port again. Called before
   * the device is made to restart, so that its reconnection isn't missed.
   */
  expectRestart(timeoutMs?: number, abortSignal?: AbortSignal): Promise<void>

  getDeviceInformation(): Promise<DeviceInformation>

  executeCommand<TResult>(command: DeviceCommand<TResult>): Promise<TResult>

  messages$: Observable<DeviceMessage>
  traffic$: Observable<DeviceMessage[]>
  logs$: Observable<LogMessage[]>
  notifications$: Observable<NotificationMessage>

  traffic: ReadonlyArray<DeviceMessage>
  logs: ReadonlyArray<LogMessage>

  clearTraffic(): void
  clearLogs(): void
  deleteLogMessage(key: number): void

  isConnected$: Observable<boolean>
  isConnected: boolean
}

