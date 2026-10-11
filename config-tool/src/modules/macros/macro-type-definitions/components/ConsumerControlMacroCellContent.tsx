import { Chip } from "@mui/material"
import type { ConsumerControlMacroDefinition } from "@src/modules/macros/models"
import { consumerControlLabel } from "./macroLabels"

export const ConsumerControlMacroCellContent = (props: {
  macro: ConsumerControlMacroDefinition
}) => {
  const { macro } = props
  const label = consumerControlLabel(macro.usageId)
  if (!label) {
    return null
  }
  return (
    <Chip
      label={<><strong>{label.name}</strong> ({label.code})</>}
      size={"small"}
      color={"primary"}
    />
  )
}
