import type { DateTime } from "luxon"

export type LogMessage = {
  key: number
  timestamp: DateTime
  level: string
  component: string | null
  message: string
}