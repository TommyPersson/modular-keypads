import type { Shortcut } from "./Shortcut"

export enum MacroDefinitionType {
  Shortcut = "Shortcut",
  ShortcutSequence = "ShortcutSequence",
  ConsumerControl = "ConsumerControl",
  HIDKeySequence = "HIDKeySequence",
}

export type MacroDefinitionBase = {
  id: number
  name: string
  type: MacroDefinitionType
}

export type ShortcutMacroDefinition = MacroDefinitionBase & {
  type: MacroDefinitionType.Shortcut
  shortcut: Shortcut
}

export type ShortcutSequenceMacroDefinition = MacroDefinitionBase & {
  type: MacroDefinitionType.ShortcutSequence
  shortcuts: Shortcut[]
}

export type ConsumerControlMacroDefinition = MacroDefinitionBase & {
  type: MacroDefinitionType.ConsumerControl
  usageId: number
}

export type HIDActionType = "Press" | "Release"

export type HIDAction = {
  type: HIDActionType,
  hidCode: number
}

// TODO Should probably be replaced with a generic sequence containing both HID and ConsumerControl (and others) actions
export type HIDKeySequenceMacroDefinition = MacroDefinitionBase & {
  type: MacroDefinitionType.HIDKeySequence
  sequence: HIDAction[]
}

export type MacroDefinition =
  | ShortcutMacroDefinition
  | ConsumerControlMacroDefinition
  | ShortcutSequenceMacroDefinition
  | HIDKeySequenceMacroDefinition