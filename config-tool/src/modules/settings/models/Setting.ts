export enum SettingType {
  UInt = "uint",
}

export type UIntSetting = {
  key: string
  type: SettingType.UInt
  value: number
  defaultValue: number
  minValue: number
  maxValue: number
}

export type Setting = UIntSetting
