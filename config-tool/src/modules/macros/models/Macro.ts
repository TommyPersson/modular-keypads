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
}

// Groups the macro types in the UI
export enum MacroTypeCategory {
  Keyboard = "Keyboard",
  Mouse = "Mouse",
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

export type SequenceStep = {
  macroId: number
  // Time to wait before running the macro, counted from when the previous step is done typing
  delayMs: number
}

// Runs other macros in order. Sequences can't be steps of other sequences.
export type SequenceMacroDefinition = MacroDefinitionBase & {
  type: MacroDefinitionType.Sequence
  steps: SequenceStep[]
}

// The same limits as on the device
export const MaxSequenceSteps = 32
export const MaxSequenceStepDelayMs = 60_000

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
  | SequenceMacroDefinition