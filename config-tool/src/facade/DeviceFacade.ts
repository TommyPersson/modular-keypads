import { Observable } from "rxjs"

export interface DeviceFacade {
  connect(): Promise<void>
  disconnect(): Promise<void>

  performPing(): Promise<void>

  $logs: Observable<LogMessage>
  $isConnected: Observable<boolean>
}

export type LogMessage = {
  direction: "to-device" | "to-host",
  message: string
}


