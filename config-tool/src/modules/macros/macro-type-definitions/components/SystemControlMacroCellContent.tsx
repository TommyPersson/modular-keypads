import { Chip } from "@mui/material"
import type { SystemControlMacroDefinition } from "@src/modules/macros/models"
import { systemControlLabel } from "./macroLabels"

export const SystemControlMacroCellContent = (props: {
  macro: SystemControlMacroDefinition
}) => {
  const { macro } = props
  const label = systemControlLabel(macro.code)
  if (!label) {
    return null
  }
  return (
    <Chip
      label={<strong>{label}</strong>}
      size={"small"}
      color={"primary"}
    />
  )
}
