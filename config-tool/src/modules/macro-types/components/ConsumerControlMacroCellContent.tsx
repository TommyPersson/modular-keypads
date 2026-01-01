import { Chip } from "@mui/material"
import { consumerControlCodes } from "@src/modules/key-bindings/data/consumerControlCodes"
import type { ConsumerControlMacroDefinition } from "@src/modules/key-bindings/models"

export const ConsumerControlMacroCellContent = (props: {
  macro: ConsumerControlMacroDefinition
}) => {
  const { macro } = props
  const code = consumerControlCodes.byUsageId[macro.usageId]
  if (!code) {
    return null
  }
  return (
    <Chip
      label={<><strong>{code.usageName}</strong> (0x{macro.usageId.toString(16)})</>}
      size={"small"}
      color={"primary"}
    />
  )
}