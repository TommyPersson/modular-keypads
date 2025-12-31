import type { DeviceCommand } from "@src/modules/device/facade/device-commands/DeviceCommand"
import { Observable } from "rxjs"
import type { DeviceInformation, NotificationMessage, RawLogMessage } from "../models"

export interface DeviceFacade {
  connect(): Promise<void>
  disconnect(): Promise<void>

  getDeviceInformation(): Promise<DeviceInformation>

  executeCommand<TResult>(command: DeviceCommand<TResult>): Promise<TResult>

  logs$: Observable<RawLogMessage>
  notifications$: Observable<NotificationMessage>

  isConnected$: Observable<boolean>
  isConnected: boolean
}

