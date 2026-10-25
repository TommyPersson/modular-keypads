import type { Shortcut } from "./Shortcut"

export enum MacroDefinitionType {
  Shortcut = "Shortcut",
  ConsumerControl = "ConsumerControl",
  SystemControl = "SystemControl",
  Text = "Text",
  MouseScroll = "MouseScroll",
  MouseButton = "MouseButton",
  MouseMovement = "MouseMovement",
  Sequence = "Sequence",
  MidiNote = "MidiNote",
  MidiRelativeControlChange = "MidiRelativeControlChange",
  MidiAbsoluteControlChange = "MidiAbsoluteControlChange",
}

// Groups the macro types in the UI
export enum MacroTypeCategory {
  Keyboard = "Keyboard",
  Mouse = "Mouse",
  Midi = "MIDI",
  Combined = "Combined",
}

export type MacroDefinitionBase = {
  id: number
  directory: string | null
  name: string
  type: MacroDefinitionType
}

export type ShortcutMacroDefinition = MacroDefinitionBase & {
  type: MacroDefinitionType.Shortcut
  shortcut: Shortcut
  // Keeps the modifiers held between repeats of the shortcut, such as for Alt + Tab
  holdModifiers: boolean
  // How long the modifiers stay held after the last repeat. Zero uses the modifier hold time setting.
  holdTimeMs: number
}

export type ConsumerControlMacroDefinition = MacroDefinitionBase & {
  type: MacroDefinitionType.ConsumerControl
  usageId: number
}

export type SystemControlMacroDefinition = MacroDefinitionBase & {
  type: MacroDefinitionType.SystemControl
  code: number
}

export type TextMacroDefinition = MacroDefinitionBase & {
  type: MacroDefinitionType.Text
  text: string
}

export type MouseScrollMacroDefinition = MacroDefinitionBase & {
  type: MacroDefinitionType.MouseScroll
  // Keyboard modifier key codes, held while scrolling
  modifiers: number[]
  // Positive scrolls up
  wheel: number
  // Positive scrolls right
  pan: number
}

export type MouseButtonMacroDefinition = MacroDefinitionBase & {
  type: MacroDefinitionType.MouseButton
  // Keyboard modifier key codes, held while clicking
  modifiers: number[]
  // Bit flags, see MouseButton
  buttons: number
}

export type MouseMovementMacroDefinition = MacroDefinitionBase & {
  type: MacroDefinitionType.MouseMovement
  // Keyboard modifier key codes, held while moving
  modifiers: number[]
  // Positive moves right
  x: number
  // Positive moves down
  y: number
}

// A note played on the USB MIDI device, with note off sent once the duration has passed, or on key release
export type MidiNoteMacroDefinition = MacroDefinitionBase & {
  type: MacroDefinitionType.MidiNote
  // 1 to 16
  channel: number
  // 0 to 127, 60 is middle C
  note: number
  // 1 to 127
  velocity: number
  // Zero sends note off right after note on. Not used when ending on release.
  durationMs: number
  // Sends note off when the key is released, rather than after the duration
  endOnRelease: boolean
}

// The same limit as on the device
export const MaxMidiNoteDurationMs = 60_000

// How relative changes are encoded in control change values, as apps expect different ones. The same numbers as on
// the device.
export enum RelativeControlChangeEncoding {
  // 64 + amount: 65 is +1, 63 is -1
  BinaryOffset = 0,
  // 1 is +1, 127 is -1
  TwosComplement = 1,
  // 1 is +1, 65 is -1
  SignMagnitude = 2,
}

// Changes the value of a controller by an amount, such as for an app slider turned with a rotary encoder
export type MidiRelativeControlChangeMacroDefinition = MacroDefinitionBase & {
  type: MacroDefinitionType.MidiRelativeControlChange
  // 1 to 16
  channel: number
  // 0 to 119, as 120 to 127 are channel mode messages
  controller: number
  // -63 to 63, positive moves up
  amount: number
  encoding: RelativeControlChangeEncoding
}

// The same limits as on the device. All the encodings can represent the amount.
export const MaxControlChangeController = 119
export const MaxRelativeControlChangeAmount = 63

// Steps the value of a controller by an amount, and sends the new value. The device keeps the value, from 0 to 127,
// starting at 0, as it doesn't know the value in the app.
export type MidiAbsoluteControlChangeMacroDefinition = MacroDefinitionBase & {
  type: MacroDefinitionType.MidiAbsoluteControlChange
  // 1 to 16
  channel: number
  // 0 to 119
  controller: number
  // -127 to 127, positive moves up
  amount: number
}

// Runs a macro, or for loop steps, goes back to an earlier step a number of times, such as to repeat a few steps
export type SequenceStep = {
  // Zero for loop steps
  macroId: number
  // Time to wait before running the macro, counted from when the previous step is done typing. For loop steps, the time
  // to wait before going back, which isn't waited once it's done going back.
  delayMs: number
  loop?: SequenceLoop
}

export type SequenceLoop = {
  // The index of the earlier step to go back to
  targetStep: number
  // How many times to go back before going on with the next step
  repeats: number
}

// Runs other macros in order. Sequences can't be steps of other sequences.
export type SequenceMacroDefinition = MacroDefinitionBase & {
  type: MacroDefinitionType.Sequence
  steps: SequenceStep[]
}

// The same limits as on the device
export const MaxSequenceSteps = 32
export const MaxSequenceStepDelayMs = 60_000
export const MaxSequenceLoopRepeats = 1_000

// Steps need a macro, and loops can only go back to an earlier step, like on the device
export function isValidSequenceStep(step: SequenceStep, index: number): boolean {
  if (step.loop) {
    return step.loop.targetStep >= 0 && step.loop.targetStep < index && step.loop.repeats > 0
  }

  return step.macroId > 0
}

export enum MouseButton {
  Left = 0x01,
  Right = 0x02,
  Middle = 0x04,
  Back = 0x08,
  Forward = 0x10,
}

export type MacroDefinition =
  | ShortcutMacroDefinition
  | ConsumerControlMacroDefinition
  | SystemControlMacroDefinition
  | TextMacroDefinition
  | MouseScrollMacroDefinition
  | MouseButtonMacroDefinition
  | MouseMovementMacroDefinition
  | MidiNoteMacroDefinition
  | MidiRelativeControlChangeMacroDefinition
  | MidiAbsoluteControlChangeMacroDefinition
  | SequenceMacroDefinition