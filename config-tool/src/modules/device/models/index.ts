export * from "./NotificationMessage"
export * from "./RawLogMessage"

export type DeviceInformation = {
  readonly deviceId: string
  readonly deviceFirmwareVersion: string
  readonly deviceType: string
  readonly deviceAddress: number
  readonly deviceName: string
  readonly deviceRegisterNames: string[]
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