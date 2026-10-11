import { Stack } from "@mui/material"
import { type MacroDefinition, MacroDefinitionType } from "@src/modules/macros/models"
import type { Meta, StoryObj } from "@storybook/react-vite"
import { useState } from "react"
import { macroTypeDefinitions } from "./index"

// Edits a local copy of the macro with the editor of its type, with the table cell content below to show the result
const StatefulMacroDefinitionEditor = (props: { initialValue: MacroDefinition }) => {
  const [value, setValue] = useState(props.initialValue)

  const typeDefinition = macroTypeDefinitions.byType[value.type]

  return (
    <Stack gap={3}>
      <typeDefinition.editorComponent value={value} onChange={setValue} />
      <typeDefinition.macroTableCellComponent macro={value} />
    </Stack>
  )
}

const meta = {
  title: "Macros/Mouse Macro Definition Editors",
  component: StatefulMacroDefinitionEditor,
} satisfies Meta<typeof StatefulMacroDefinitionEditor>

export default meta

type Story = StoryObj<typeof meta>

export const Scroll: Story = {
  args: {
    initialValue: {
      id: 1,
      directory: null,
      name: "Scroll",
      type: MacroDefinitionType.MouseScroll,
      wheel: -3,
      pan: 0,
    },
  },
}

export const Button: Story = {
  args: {
    initialValue: {
      id: 1,
      directory: null,
      name: "Button",
      type: MacroDefinitionType.MouseButton,
      buttons: 0x05,
    },
  },
}

export const Movement: Story = {
  args: {
    initialValue: {
      id: 1,
      directory: null,
      name: "Movement",
      type: MacroDefinitionType.MouseMovement,
      x: 10,
      y: -20,
    },
  },
}
