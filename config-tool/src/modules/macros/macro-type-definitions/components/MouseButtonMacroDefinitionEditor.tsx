import { Checkbox, FormControl, FormControlLabel, FormGroup, FormLabel, Stack } from "@mui/material"
import { mouseButtons } from "@src/modules/macros/data/mouseButtons"
import type { MouseButton, MouseButtonMacroDefinition } from "@src/modules/macros/models"
import { useCallback } from "react"
import { ModifiersEditor } from "./ShortcutMacroDefinitionEditor"

export const MouseButtonMacroDefinitionEditor = (props: {
  value: MouseButtonMacroDefinition
  onChange: (value: MouseButtonMacroDefinition) => void
}) => {
  const { value, onChange } = props

  const handleButtonToggle = useCallback((button: MouseButton, checked: boolean) => {
    onChange({ ...value, buttons: checked ? value.buttons | button : value.buttons & ~button })
  }, [value, onChange])

  return (
    <Stack gap={3}>
      <ModifiersEditor
        values={value.modifiers}
        onChange={modifiers => onChange({ ...value, modifiers })}
      />
      <FormControl component={"fieldset"} sx={{ minWidth: 400 }}>
        <FormLabel component={"legend"}>Click</FormLabel>
        <FormGroup row>
          {mouseButtons.map(({ button, name }) => (
            <FormControlLabel
              key={button}
              label={name}
              control={
                <Checkbox
                  checked={(value.buttons & button) !== 0}
                  onChange={(_, checked) => handleButtonToggle(button, checked)}
                />
              }
            />
          ))}
        </FormGroup>
      </FormControl>
    </Stack>
  )
}
