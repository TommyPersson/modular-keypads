import { Chip } from "@mui/material"
import type { MouseScrollMacroDefinition } from "@src/modules/macros/models"

export const MouseScrollMacroCellContent = (props: {
  macro: MouseScrollMacroDefinition
}) => {
  const { macro } = props

  const directions: string[] = []
  if (macro.wheel !== 0) {
    directions.push(`${macro.wheel > 0 ? "up" : "down"} ${Math.abs(macro.wheel)}`)
  }
  if (macro.pan !== 0) {
    directions.push(`${macro.pan > 0 ? "right" : "left"} ${Math.abs(macro.pan)}`)
  }
  if (directions.length === 0) {
    return null
  }

  return (
    <Chip
      label={<strong>{`Scroll ${directions.join(", ")}`}</strong>}
      size={"small"}
      color={"primary"}
    />
  )
}
