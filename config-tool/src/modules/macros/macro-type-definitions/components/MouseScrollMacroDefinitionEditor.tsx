import { Stack } from "@mui/material"
import type { MouseScrollMacroDefinition } from "@src/modules/macros/models"
import { ModifiersEditor } from "./ShortcutMacroDefinitionEditor"
import { IntegerField } from "./IntegerField"
import { IntegerType } from "./IntegerType"

export const MouseScrollMacroDefinitionEditor = (props: {
  value: MouseScrollMacroDefinition
  onChange: (value: MouseScrollMacroDefinition) => void
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
          type={IntegerType.S8}
          label={"Scroll vertically"}
          helperText={"Positive scrolls up"}
          value={value.wheel}
          onChange={wheel => onChange({ ...value, wheel })}
        />
        <IntegerField
          type={IntegerType.S8}
          label={"Scroll horizontally"}
          helperText={"Positive scrolls right"}
          value={value.pan}
          onChange={pan => onChange({ ...value, pan })}
        />
      </Stack>
    </Stack>
  )
}
