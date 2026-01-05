import type { Shortcut } from "./Shortcut"

export enum MacroDefinitionType {
  Shortcut = "Shortcut",
  ConsumerControl = "ConsumerControl",
  SystemControl = "SystemControl",
  Text = "Text",
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

export type MacroDefinition =
  | ShortcutMacroDefinition
  | ConsumerControlMacroDefinition
  | SystemControlMacroDefinition
  | TextMacroDefinition