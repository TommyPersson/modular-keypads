export enum KeyBindingTriggerType {
  PushButton = "PushButton",
  RotaryEncoder = "RotaryEncoder",
}

// The button state that triggers the binding, so a button can have a binding for each
export enum PushButtonEvent {
  Pressed = "Pressed",
  LongPressed = "LongPressed",
}

export enum RotaryEncoderDirection {
  Clockwise = "Clockwise",
  CounterClockwise = "CounterClockwise",
}

export type KeyBindingTriggerBase = {
  deviceId: string
  type: KeyBindingTriggerType
}

export type PushButtonKeyBindingTrigger = KeyBindingTriggerBase & {
  type: KeyBindingTriggerType.PushButton,
  number: number
  event: PushButtonEvent
}

export type RotaryEncoderKeyBindingTrigger = KeyBindingTriggerBase & {
  type: KeyBindingTriggerType.RotaryEncoder,
  number: number
  direction: RotaryEncoderDirection
}

export type KeyBindingTrigger = PushButtonKeyBindingTrigger | RotaryEncoderKeyBindingTrigger

export type KeyBinding = {
  macroId: number
  trigger: KeyBindingTrigger
}