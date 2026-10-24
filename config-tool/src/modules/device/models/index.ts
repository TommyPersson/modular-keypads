export * from "./NotificationMessage"
export * from "./DeviceMessage"
export * from "./LogMessage"

export type DeviceInformation = {
  readonly deviceId: string
  // The build date and git commit of the firmware, e.g. "2026-10-24-2444b54", or null when it isn't known, as for
  // sub-devices, and for devices whose firmware doesn't report it yet
  readonly deviceFirmwareVersion: string | null
  readonly deviceType: string
  readonly deviceAddress: number
  readonly deviceName: string
  readonly deviceRegisterNames: string[]
  readonly parentDeviceId: string | null
  // Position on the parent device, starting at 1, or null for devices that aren't sub-devices
  readonly subDeviceSlot: number | null
}

export type DeviceRegisterValue = {
  name: string
  value: number
}

export type BaseCapability = {
  type: string
  index: number
}

export type PushButtonCapability = BaseCapability & {
  type: "PushButton"
  number: number
  hasLed: boolean
}

export type RotaryEncoderCapability = BaseCapability & {
  type: "RotaryEncoder"
  number: number
}

export type DeviceCapability = PushButtonCapability | RotaryEncoderCapability

export type DeviceMetric = {
  name: string,
  value: number
}

export class DeviceCommandError extends Error {
  constructor(
    readonly commandId: number,
    readonly errorCode: string
  ) {
    super(errorCode)
  }
}