import { Chip } from "@mui/material"
import type { MouseScrollMacroDefinition } from "@src/modules/macros/models"
import { mouseScrollLabel } from "./macroLabels"
import { ModifiersVisualization } from "./ModifiersVisualization"

export const MouseScrollMacroCellContent = (props: {
  macro: MouseScrollMacroDefinition
}) => {
  const { macro } = props

  const label = mouseScrollLabel(macro.wheel, macro.pan)
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
