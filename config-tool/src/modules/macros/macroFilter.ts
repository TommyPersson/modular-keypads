import { macroTypeDefinitions } from "@src/modules/macros/macro-type-definitions"
import type { MacroDefinition } from "@src/modules/macros/models"

/**
 * Creates a check for the macros that match all words of the filter, or null when the filter has no words. The words
 * can be found in the name, the directory or the definition as it's shown, e.g. a modifier or a key code. The macros
 * are all macros, which definitions can refer to, such as the steps of a sequence.
 */
export function createMacroFilter(
  filter: string,
  macros: MacroDefinition[],
): ((macro: MacroDefinition) => boolean) | null {
  const words = filter.trim().toLocaleLowerCase().split(/\s+/).filter(it => it.length > 0)
  if (words.length === 0) {
    return null
  }

  return (macro: MacroDefinition) => {
    const searchTexts = macroTypeDefinitions.byType[macro.type]?.searchTexts(macro, macros) ?? []
    const text = [macro.name, macro.directory ?? "", ...searchTexts].join("\n").toLocaleLowerCase()
    return words.every(word => text.includes(word))
  }
}
