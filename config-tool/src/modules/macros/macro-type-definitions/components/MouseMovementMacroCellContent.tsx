import { Chip } from "@mui/material"
import type { MouseMovementMacroDefinition } from "@src/modules/macros/models"
import { mouseMovementLabel } from "./macroLabels"
import { ModifiersVisualization } from "./ModifiersVisualization"

export const MouseMovementMacroCellContent = (props: {
  macro: MouseMovementMacroDefinition
}) => {
  const { macro } = props

  const label = mouseMovementLabel(macro.x, macro.y)
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
