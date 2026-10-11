import { Chip } from "@mui/material"
import type { MouseButtonMacroDefinition } from "@src/modules/macros/models"
import { mouseButtonLabel } from "./macroLabels"
import { ModifiersVisualization } from "./ModifiersVisualization"

export const MouseButtonMacroCellContent = (props: {
  macro: MouseButtonMacroDefinition
}) => {
  const { macro } = props

  const label = mouseButtonLabel(macro.buttons)
  if (!label) {
    return null
  }

  return (
    <ModifiersVisualization modifiers={macro.modifiers}>
      <Chip
        label={<strong>{label}</strong>}
        size={"small"}
        color={"primary"}
      />
    </ModifiersVisualization>
  )
}
