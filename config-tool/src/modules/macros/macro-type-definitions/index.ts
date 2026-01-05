import { TextMacroTypeDefinition } from "@src/modules/macros/macro-type-definitions/TextMacroTypeDefinition"
import type { MacroDefinition, MacroTypeDefinition } from "@src/modules/macros/models"
import { ConsumerControlMacroTypeDefinition } from "./ConsumerControlMacroTypeDefinition"
import { ShortcutMacroTypeDefinition } from "./ShortcutMacroTypeDefinition"
import { SystemControlTypeDefinition } from "./SystemControlTypeDefinition"


const allMacroTypeDefinitions: MacroTypeDefinition<MacroDefinition>[] = [
  ShortcutMacroTypeDefinition as any,
  ConsumerControlMacroTypeDefinition as any,
  SystemControlTypeDefinition as any,
  TextMacroTypeDefinition as any,
]

export type MacroTypeDefinitions = {
  all: MacroTypeDefinition<MacroDefinition>[]
  byType: { [index: string]: MacroTypeDefinition<MacroDefinition> }
  byCode: { [index: number]: MacroTypeDefinition<MacroDefinition> }
}

export const macroTypeDefinitions: MacroTypeDefinitions = {
  all: allMacroTypeDefinitions,
  byType: allMacroTypeDefinitions
    .reduce((acc, curr) => ({ ...acc, [curr.type.toString()]: curr }), {}),
  byCode: allMacroTypeDefinitions
    .reduce((acc, curr) => ({ ...acc, [curr.numericCode]: curr }), {})
}