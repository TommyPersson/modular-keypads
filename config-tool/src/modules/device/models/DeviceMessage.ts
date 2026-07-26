import type { DateTime } from "luxon"

export type DeviceMessage = {
  direction: "to-device" | "to-host"
  message: string
  timestamp: DateTime
}

