import type { DateTime } from "luxon"

export type NotificationMessage = {
  deviceId: string
  type: string
  args: string[]
  timestamp: DateTime
}