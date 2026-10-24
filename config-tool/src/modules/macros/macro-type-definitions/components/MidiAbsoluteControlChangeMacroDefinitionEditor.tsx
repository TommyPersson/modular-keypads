import { Chip, Stack } from "@mui/material"
import { midiControllerName } from "@src/modules/macros/data/midiControllers"
import { MaxControlChangeController, type MidiAbsoluteControlChangeMacroDefinition } from "@src/modules/macros/models"
import { IntegerField } from "./IntegerField"
import { IntegerType } from "./IntegerType"

export const MidiAbsoluteControlChangeMacroDefinitionEditor = (props: {
  value: MidiAbsoluteControlChangeMacroDefinition
  onChange: (value: MidiAbsoluteControlChangeMacroDefinition) => void
}) => {
  const { value, onChange } = props

  return (
    <Stack gap={2} sx={{ minWidth: 400 }}>
      <Stack direction={"row"} gap={2}>
        <IntegerField
          type={IntegerType.U8}
          max={MaxControlChangeController}
          label={"Controller"}
          helperText={`0 to ${MaxControlChangeController}`}
          endContent={<Chip label={midiControllerName(value.controller)} size={"small"} />}
          value={value.controller}
          onChange={controller => onChange({ ...value, controller })}
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
          type={IntegerType.S8}
          label={"Step"}
          helperText={"Added to the value, which goes from 0 to 127"}
          value={value.amount}
          onChange={amount => onChange({ ...value, amount })}
        />
        <Stack sx={{ width: "100%" }} />
      </Stack>
    </Stack>
  )
}
