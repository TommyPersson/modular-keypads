import type { MacroDefinition } from "@src/modules/key-bindings/models"
import { ShortcutMacroTypeDefinition } from "./ShortcutMacroTypeDefinition"
import { ConsumerControlMacroTypeDefinition } from "./ConsumerControlMacroTypeDefinition"
import type { MacroTypeDefinition } from "./MacroTypeDefinition"
import { SystemControlTypeDefinition } from "./SystemControlTypeDefinition"

export const allMacroTypeDefinitions: MacroTypeDefinition<MacroDefinition>[] = [
  ShortcutMacroTypeDefinition as any,
  ConsumerControlMacroTypeDefinition as any,
  SystemControlTypeDefinition as any,
]

export const macroTypeDefinitionsByType: { [index: string]: MacroTypeDefinition<MacroDefinition> } =
  allMacroTypeDefinitions.reduce((acc, curr) => ({ ...acc, [curr.type.toString()]: curr }), {})

export const macroTypeDefinitionsByCode: { [index: number]: MacroTypeDefinition<MacroDefinition> } =
  allMacroTypeDefinitions.reduce((acc, curr) => ({ ...acc, [curr.numericCode]: curr }), {})