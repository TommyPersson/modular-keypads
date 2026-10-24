import { Chip, Stack } from "@mui/material"
import type { MidiRelativeControlChangeMacroDefinition } from "@src/modules/macros/models"
import { midiChannelLabel, midiRelativeControlChangeLabel } from "./macroLabels"

export const MidiRelativeControlChangeMacroCellContent = (props: {
  macro: MidiRelativeControlChangeMacroDefinition
}) => {
  const { macro } = props

  return (
    <Stack direction={"row"} gap={0.5} alignItems={"center"}>
      <Chip
        label={<strong>{midiRelativeControlChangeLabel(macro.controller, macro.amount)}</strong>}
        size={"small"}
        color={"primary"}
      />
      <Chip
        label={midiChannelLabel(macro.channel)}
        size={"small"}
        variant={"outlined"}
      />
    </Stack>
  )
}
