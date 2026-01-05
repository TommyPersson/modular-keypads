import { Typography } from "@mui/material"
import type { TextMacroDefinition } from "@src/modules/macros/models"

export const TextMacroCellContent = (props: {
  macro: TextMacroDefinition
}) => {
  const { macro } = props

  return (
    <Typography noWrap variant={"body2"}>
      {macro.text}
    </Typography>
  )
}