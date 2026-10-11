import { macroTypeDefinitions } from "@src/modules/macros/macro-type-definitions"
import type { MacroDefinition } from "@src/modules/macros/models"

/**
 * Shows what the macro does, e.g. the keys of a shortcut, as defined by its type.
 */
export const MacroDefinitionVisualization = (props: {
  macro: MacroDefinition
}) => {
  const { macro } = props

  const typeDefinition = macroTypeDefinitions.byType[macro.type]
  if (typeDefinition) {
    return <typeDefinition.macroTableCellComponent macro={macro} />
  }

  return null
}
