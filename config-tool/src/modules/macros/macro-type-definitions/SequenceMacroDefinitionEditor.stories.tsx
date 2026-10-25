import { Stack } from "@mui/material"
import { type MacroDefinition, MacroDefinitionType } from "@src/modules/macros/models"
import { macros } from "@src/modules/overview/ui/components/DevicePreviewCard/DevicePreviewCard.storyData"
import { withMockDevice } from "@src/storybook"
import type { Meta, StoryObj } from "@storybook/react-vite"
import { useState } from "react"
import { macroTypeDefinitions } from "./index"

// Edits a local copy of the macro with the editor of its type, with the table cell content below to show the result
const StatefulMacroDefinitionEditor = (props: { initialValue: MacroDefinition }) => {
  const [value, setValue] = useState(props.initialValue)

  const typeDefinition = macroTypeDefinitions.byType[value.type]

  return (
    <Stack gap={3} sx={{ width: 640 }}>
      <typeDefinition.editorComponent value={value} onChange={setValue} />
      <typeDefinition.macroTableCellComponent macro={value} />
    </Stack>
  )
}

const meta = {
  title: "Macros/Sequence Macro Definition Editor",
  component: StatefulMacroDefinitionEditor,
  decorators: [withMockDevice],
  // The stored macros, which the steps refer to by ID
  parameters: { mockDevice: { devices: [], macros } },
} satisfies Meta<typeof StatefulMacroDefinitionEditor>

export default meta

type Story = StoryObj<typeof meta>

// Repeats the last two steps three more times, waiting a bit before each repeat
export const WithLoop: Story = {
  args: {
    initialValue: {
      id: 100,
      directory: null,
      name: "Repeat",
      type: MacroDefinitionType.Sequence,
      steps: [
        { macroId: 1, delayMs: 0 },
        { macroId: 2, delayMs: 0 },
        { macroId: 5, delayMs: 250 },
        { macroId: 0, delayMs: 500, loop: { targetStep: 1, repeats: 3 } },
      ],
    },
  },
}

// A loop that was moved above the step it goes back to
export const WithInvalidLoop: Story = {
  args: {
    initialValue: {
      id: 100,
      directory: null,
      name: "Repeat",
      type: MacroDefinitionType.Sequence,
      steps: [
        { macroId: 1, delayMs: 0 },
        { macroId: 0, delayMs: 0, loop: { targetStep: 2, repeats: 2 } },
        { macroId: 2, delayMs: 0 },
      ],
    },
  },
}
