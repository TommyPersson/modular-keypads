import { Chip, ListItemText, MenuItem, Stack, TextField } from "@mui/material"
import { midiControllerName } from "@src/modules/macros/data/midiControllers"
import {
  MaxControlChangeController,
  MaxRelativeControlChangeAmount,
  type MidiRelativeControlChangeMacroDefinition,
  RelativeControlChangeEncoding
} from "@src/modules/macros/models"
import { IntegerField } from "./IntegerField"
import { IntegerType } from "./IntegerType"

const ENCODINGS: { encoding: RelativeControlChangeEncoding, name: string, example: string }[] = [
  { encoding: RelativeControlChangeEncoding.BinaryOffset, name: "Binary offset", example: "65 is +1, 63 is -1" },
  { encoding: RelativeControlChangeEncoding.TwosComplement, name: "Two's complement", example: "1 is +1, 127 is -1" },
  { encoding: RelativeControlChangeEncoding.SignMagnitude, name: "Sign and magnitude", example: "1 is +1, 65 is -1" },
]

export const MidiRelativeControlChangeMacroDefinitionEditor = (props: {
  value: MidiRelativeControlChangeMacroDefinition
  onChange: (value: MidiRelativeControlChangeMacroDefinition) => void
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
          min={-MaxRelativeControlChangeAmount}
          max={MaxRelativeControlChangeAmount}
          label={"Amount"}
          helperText={"Positive moves up, negative moves down"}
          value={value.amount}
          onChange={amount => onChange({ ...value, amount })}
        />
        <TextField
          select
          variant={"filled"}
          label={"Encoding"}
          helperText={"As the app expects it"}
          value={value.encoding}
          onChange={event => onChange({ ...value, encoding: Number(event.target.value) })}
          slotProps={{
            select: {
              renderValue: selected => ENCODINGS.find(it => it.encoding === selected)?.name,
            },
          }}
          fullWidth
        >
          {ENCODINGS.map(({ encoding, name, example }) => (
            <MenuItem key={encoding} value={encoding}>
              <ListItemText primary={name} secondary={example} />
            </MenuItem>
          ))}
        </TextField>
      </Stack>
    </Stack>
  )
}
