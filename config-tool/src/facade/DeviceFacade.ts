import type { DateTime } from "luxon"
import { Observable } from "rxjs"

export interface DeviceFacade {
  connect(): Promise<void>
  disconnect(): Promise<void>

  performPing(): Promise<string>

  $logs: Observable<LogMessage>
  $isConnected: Observable<boolean>
}

export type LogMessage = {
  direction: "to-device" | "to-host"
  message: string
  timestamp: DateTime
}


