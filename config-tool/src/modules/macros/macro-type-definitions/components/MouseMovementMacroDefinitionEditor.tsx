import { Stack } from "@mui/material"
import type { MouseMovementMacroDefinition } from "@src/modules/macros/models"
import { ModifiersEditor } from "./ShortcutMacroDefinitionEditor"
import { IntegerField } from "./IntegerField"
import { IntegerType } from "./IntegerType"

export const MouseMovementMacroDefinitionEditor = (props: {
  value: MouseMovementMacroDefinition
  onChange: (value: MouseMovementMacroDefinition) => void
}) => {
  const { value, onChange } = props

  return (
    <Stack gap={3}>
      <ModifiersEditor
        values={value.modifiers}
        onChange={modifiers => onChange({ ...value, modifiers })}
      />
      <Stack direction={"row"} gap={2} sx={{ minWidth: 400 }}>
        <IntegerField
          type={IntegerType.S16}
          label={"Move horizontally"}
          helperText={"Positive moves right"}
          value={value.x}
          onChange={x => onChange({ ...value, x })}
        />
        <IntegerField
          type={IntegerType.S16}
          label={"Move vertically"}
          helperText={"Positive moves down"}
          value={value.y}
          onChange={y => onChange({ ...value, y })}
        />
      </Stack>
    </Stack>
  )
}
