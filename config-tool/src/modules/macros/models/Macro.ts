import type { Shortcut } from "./Shortcut"

export enum MacroDefinitionType {
  Shortcut = "Shortcut",
  ConsumerControl = "ConsumerControl",
  SystemControl = "SystemControl",
  Text = "Text",
  MouseScroll = "MouseScroll",
  MouseButton = "MouseButton",
  MouseMovement = "MouseMovement",
}

// Groups the macro types in the UI
export enum MacroTypeCategory {
  Keyboard = "Keyboard",
  Mouse = "Mouse",
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