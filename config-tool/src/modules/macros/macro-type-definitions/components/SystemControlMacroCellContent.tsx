import { Chip } from "@mui/material"
import { systemControlCodes } from "@src/modules/macros/data/systemControlCodes"
import type { SystemControlMacroDefinition } from "@src/modules/macros/models"

export const SystemControlMacroCellContent = (props: {
  macro: SystemControlMacroDefinition
}) => {
  const { macro } = props
  const code = systemControlCodes.byCode[macro.code]
  if (!code) {
    return null
  }
  return (
    <Chip
      label={<><strong>{code.description}</strong></>}
      size={"small"}
      color={"primary"}
    />
  )
}