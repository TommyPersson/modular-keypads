import { Stack } from "@mui/material"
import type { MouseMovementMacroDefinition } from "@src/modules/macros/models"
import { SignedByteField } from "./SignedByteField"

export const MouseMovementMacroDefinitionEditor = (props: {
  value: MouseMovementMacroDefinition
  onChange: (value: MouseMovementMacroDefinition) => void
}) => {
  const { value, onChange } = props

  return (
    <Stack direction={"row"} gap={2} sx={{ minWidth: 400 }}>
      <SignedByteField
        label={"Move horizontally"}
        helperText={"Positive moves right"}
        value={value.x}
        onChange={x => onChange({ ...value, x })}
      />
      <SignedByteField
        label={"Move vertically"}
        helperText={"Positive moves down"}
        value={value.y}
        onChange={y => onChange({ ...value, y })}
      />
    </Stack>
  )
}
