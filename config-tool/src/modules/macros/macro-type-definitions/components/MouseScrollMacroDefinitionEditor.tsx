import { Stack } from "@mui/material"
import type { MouseScrollMacroDefinition } from "@src/modules/macros/models"
import { SignedByteField } from "./SignedByteField"

export const MouseScrollMacroDefinitionEditor = (props: {
  value: MouseScrollMacroDefinition
  onChange: (value: MouseScrollMacroDefinition) => void
}) => {
  const { value, onChange } = props

  return (
    <Stack direction={"row"} gap={2} sx={{ minWidth: 400 }}>
      <SignedByteField
        label={"Scroll vertically"}
        helperText={"Positive scrolls up"}
        value={value.wheel}
        onChange={wheel => onChange({ ...value, wheel })}
      />
      <SignedByteField
        label={"Scroll horizontally"}
        helperText={"Positive scrolls right"}
        value={value.pan}
        onChange={pan => onChange({ ...value, pan })}
      />
    </Stack>
  )
}
