import { Chip, Stack } from "@mui/material"
import type { RandomTextMacroDefinition } from "@src/modules/macros/models"
import { randomTextFormatLabel, randomTextQuotedLabel } from "./macroLabels"

export const RandomTextMacroCellContent = (props: {
  macro: RandomTextMacroDefinition
}) => {
  const { macro } = props
  const label = randomTextFormatLabel(macro.format)
  const quotedLabel = randomTextQuotedLabel(macro.quoted)
  if (!label) {
    return null
  }
  return (
    <Stack direction={"row"} gap={0.5} alignItems={"center"}>
      <Chip
        label={<strong>{label}</strong>}
        size={"small"}
        color={"primary"}
      />
      {quotedLabel && (
        <Chip
          label={quotedLabel}
          size={"small"}
          variant={"outlined"}
        />
      )}
    </Stack>
  )
}
