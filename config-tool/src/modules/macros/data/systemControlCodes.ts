import type { SystemControlCode } from "../models"

// These codes are specified by the ESP library, rather than the actual USB HID spec.

export const allSystemControlCodes: SystemControlCode[] = [
  { code: 0x01, description: "Power Off" },
  { code: 0x02, description: "Standby (Sleep)" },
  { code: 0x03, description: "Wake Host" },
]

export type SystemControlCodes = {
  all: SystemControlCode[]
  byCode: { [usageId: number]: SystemControlCode }
}

export const systemControlCodes: SystemControlCodes = {
  all: allSystemControlCodes,
  byCode: allSystemControlCodes
    .reduce((acc, curr) => ({ ...acc, [curr.code]: curr }), {}),
}
