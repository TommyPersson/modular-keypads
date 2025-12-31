import type { DateTime } from "luxon"

export type RawLogMessage = {
  direction: "to-device" | "to-host"
  message: string
  timestamp: DateTime
}