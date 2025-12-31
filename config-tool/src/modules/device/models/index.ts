
export type DeviceInformation = {
  readonly deviceId: string
  readonly deviceFirmwareVersion: string
  readonly deviceType: string
  readonly deviceAddress: number
  readonly deviceName: string
  readonly deviceRegisterNames: string[]
}

export type DeviceRegisterValues = { [index: string]: number }

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