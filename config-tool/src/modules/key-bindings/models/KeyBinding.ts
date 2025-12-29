export enum KeyBindingTriggerType {
  PushButton = "PushButton",
  RotaryEncoder = "RotaryEncoder",
}

export enum RotaryEncoderDirection {
  Clockwise = "Clockwise",
  CounterClockwise = "CounterClockwise",
}

export type KeyBindingTriggerBase<TTriggerType> = {
  deviceId: string
  type: TTriggerType
}

export type PushButtonKeyBindingTrigger = KeyBindingTriggerBase<KeyBindingTriggerType.PushButton> & {
  type: KeyBindingTriggerType.PushButton,
  number: number
}

export type RotaryEncoderKeyBindingTrigger = KeyBindingTriggerBase<KeyBindingTriggerType.RotaryEncoder> & {
  type: KeyBindingTriggerType.RotaryEncoder,
  number: number
  direction: RotaryEncoderDirection
}

export type KeyBindingTrigger = PushButtonKeyBindingTrigger | RotaryEncoderKeyBindingTrigger

export type KeyBinding = {
  macroId: number
  trigger: KeyBindingTrigger
}