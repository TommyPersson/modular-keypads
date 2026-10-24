import { Checkbox, FormControl, FormControlLabel, FormHelperText, Stack } from "@mui/material"
import { MaxMidiNoteDurationMs, type MidiNoteMacroDefinition } from "@src/modules/macros/models"
import { IntegerField } from "./IntegerField"
import { IntegerType } from "./IntegerType"
import { midiNoteName } from "./macroLabels"

export const MidiNoteMacroDefinitionEditor = (props: {
  value: MidiNoteMacroDefinition
  onChange: (value: MidiNoteMacroDefinition) => void
}) => {
  const { value, onChange } = props

  return (
    <Stack gap={2} sx={{ minWidth: 400 }}>
      <Stack direction={"row"} gap={2}>
        <IntegerField
          type={IntegerType.U8}
          max={127}
          label={"Note"}
          helperText={`${midiNoteName(value.note)} in note names`}
          value={value.note}
          onChange={note => onChange({ ...value, note })}
        />
        <IntegerField
          type={IntegerType.U8}
          min={1}
          max={16}
          label={"Channel"}
          helperText={"1 to 16"}
          value={value.channel}
          onChange={channel => onChange({ ...value, channel })}
        />
      </Stack>
      <Stack direction={"row"} gap={2}>
        <IntegerField
          type={IntegerType.U8}
          min={1}
          max={127}
          label={"Velocity"}
          helperText={"Some apps use it as a value"}
          value={value.velocity}
          onChange={velocity => onChange({ ...value, velocity })}
        />
        <IntegerField
          type={IntegerType.U16}
          max={MaxMidiNoteDurationMs}
          label={"Duration (ms)"}
          helperText={"Time until note off"}
          disabled={value.endOnRelease}
          value={value.durationMs}
          onChange={durationMs => onChange({ ...value, durationMs })}
        />
      </Stack>
      <FormControl>
        <FormControlLabel
          label={"End on key release"}
          control={
            <Checkbox
              checked={value.endOnRelease}
              onChange={(_, endOnRelease) => onChange({ ...value, endOnRelease })}
            />
          }
        />
        <FormHelperText>
          Keeps the note playing while the key is held, for apps with actions that last as long as the note, such as
          push to talk.
        </FormHelperText>
      </FormControl>
    </Stack>
  )
}
