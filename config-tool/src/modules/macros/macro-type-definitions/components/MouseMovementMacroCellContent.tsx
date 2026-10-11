import { Chip } from "@mui/material"
import type { MouseMovementMacroDefinition } from "@src/modules/macros/models"
import { ModifiersVisualization } from "./ModifiersVisualization"

export const MouseMovementMacroCellContent = (props: {
  macro: MouseMovementMacroDefinition
}) => {
  const { macro } = props

  if (macro.x === 0 && macro.y === 0) {
    return null
  }

  return (
    <ModifiersVisualization modifiers={macro.modifiers}>
      <Chip
        label={<strong>{`Move ${macro.x}, ${macro.y}`}</strong>}
        size={"small"}
        color={"primary"}
      />
    </ModifiersVisualization>
  )
}
