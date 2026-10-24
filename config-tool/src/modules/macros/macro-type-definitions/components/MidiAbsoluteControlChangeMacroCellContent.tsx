import { Chip, Stack } from "@mui/material"
import type { MidiAbsoluteControlChangeMacroDefinition } from "@src/modules/macros/models"
import { midiAbsoluteControlChangeLabel, midiChannelLabel } from "./macroLabels"

export const MidiAbsoluteControlChangeMacroCellContent = (props: {
  macro: MidiAbsoluteControlChangeMacroDefinition
}) => {
  const { macro } = props

  return (
    <Stack direction={"row"} gap={0.5} alignItems={"center"}>
      <Chip
        label={<strong>{midiAbsoluteControlChangeLabel(macro.controller, macro.amount)}</strong>}
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
