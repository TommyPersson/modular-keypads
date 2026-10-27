import { Checkbox, FormControl, FormControlLabel, FormHelperText, ListItemText, MenuItem, Stack, TextField } from "@mui/material"
import type { RandomTextMacroDefinition } from "@src/modules/macros/models"
import { randomTextFormatLabel, randomTextFormats } from "./macroLabels"

export const RandomTextMacroDefinitionEditor = (props: {
  value: RandomTextMacroDefinition
  onChange: (value: RandomTextMacroDefinition) => void
}) => {
  const { value, onChange } = props

  return (
    <Stack gap={2} sx={{ minWidth: 400 }}>
      <TextField
        select
        variant={"filled"}
        label={"Format"}
        value={value.format}
        onChange={event => onChange({ ...value, format: Number(event.target.value) })}
        slotProps={{
          select: {
            renderValue: selected => randomTextFormatLabel(selected as number),
          },
        }}
      >
        {randomTextFormats.map(({ format, name, example }) => (
          <MenuItem key={format} value={format}>
            <ListItemText primary={name} secondary={example} />
          </MenuItem>
        ))}
      </TextField>
      <FormControl>
        <FormControlLabel
          label={"Wrap in quotes"}
          control={
            <Checkbox
              checked={value.quoted}
              onChange={(_, quoted) => onChange({ ...value, quoted })}
            />
          }
        />
        <FormHelperText>
          Types the text in double quotes, such as for pasting into code or JSON.
        </FormHelperText>
      </FormControl>
    </Stack>
  )
}
