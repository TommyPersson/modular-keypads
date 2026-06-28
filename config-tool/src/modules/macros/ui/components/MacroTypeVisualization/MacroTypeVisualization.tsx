import { macroTypeDefinitions } from "@src/modules/macros/macro-type-definitions"
import { MacroDefinitionType } from "@src/modules/macros/models"

export const MacroTypeVisualization = (props: {
  type: MacroDefinitionType
}) => {
  const { type } = props

  const typeDefinition = macroTypeDefinitions.byType[type]
  if (typeDefinition) {
    return <typeDefinition.typeVisualizationComponent />
  }

  return null
}

