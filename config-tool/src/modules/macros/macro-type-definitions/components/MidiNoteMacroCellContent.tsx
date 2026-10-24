import { Chip, Stack } from "@mui/material"
import type { MidiNoteMacroDefinition } from "@src/modules/macros/models"
import { midiChannelLabel, midiNoteLabel } from "./macroLabels"

export const MidiNoteMacroCellContent = (props: {
  macro: MidiNoteMacroDefinition
}) => {
  const { macro } = props

  return (
    <Stack direction={"row"} gap={0.5} alignItems={"center"}>
      <Chip
        label={<strong>{midiNoteLabel(macro.note)}</strong>}
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
