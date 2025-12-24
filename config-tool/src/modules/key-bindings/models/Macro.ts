import type { Shortcut } from "./Shortcut"

export type MacroDefinitionType = "Shortcut" | "ShortcutSequence" | "HIDKeySequence"

export type MacroDefinitionBase = {
  id: string
  type: MacroDefinitionType
}

export type ShortcutMacroDefinition = MacroDefinitionBase & {
  type: "Shortcut"
  shortcut: Shortcut
}

export type ShortcutSequenceMacroDefinition = MacroDefinitionBase & {
  type: "ShortcutSequence"
  shortcuts: Shortcut[]
}

export type HIDActionType = "Press" | "Release"

export type HIDAction = {
  type: HIDActionType,
  hidCode: number
}

// TODO Should probably be replaced with a generic sequence containing both HID and ConsumerControl (and others) actions
export type HIDKeySequenceMacroDefinition = MacroDefinitionBase & {
  type: "HIDKeySequence"
  sequence: HIDAction[]
}

export type MacroDefinition =
  | ShortcutMacroDefinition
  | ShortcutSequenceMacroDefinition
  | HIDKeySequenceMacroDefinition