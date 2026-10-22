import { Box, Stack, Typography } from "@mui/material"
import { type MacroDefinition, MacroDefinitionType } from "@src/modules/macros/models"
import { macros } from "@src/modules/overview/ui/components/DevicePreviewCard/DevicePreviewCard.storyData"
import { withMockDevice } from "@src/storybook"
import type { Meta, StoryObj } from "@storybook/react-vite"
import { MacroDefinitionVisualization } from "./MacroDefinitionVisualization"

// As wide as the definition column of the macros pane, outlined to show where previews that don't fit are cut off
const PREVIEW_WIDTH = 400

const meta = {
  title: "Macros/Macro Definition Previews",
  component: MacroDefinitionVisualization,
  render: (args) => (
    <Box sx={{ width: PREVIEW_WIDTH, outline: "1px dashed gray", display: "flex", minWidth: 0 }}>
      <MacroDefinitionVisualization {...args} />
    </Box>
  ),
  decorators: [withMockDevice],
  // The stored macros, which the sequences refer to by ID
  parameters: { mockDevice: { devices: [], macros } },
} satisfies Meta<typeof MacroDefinitionVisualization>

export default meta

type Story = StoryObj<typeof meta>

const base = { id: 100, directory: null, name: "Preview" }

const story = (macro: MacroDefinition): Story => ({ args: { macro } })

export const Shortcut = story({
  ...base,
  type: MacroDefinitionType.Shortcut,
  shortcut: { modifiers: [0xe0], hidCode: 0x06 },
  holdModifiers: false,
  holdTimeMs: 0,
})

export const ShortcutWithoutModifiers = story({
  ...base,
  type: MacroDefinitionType.Shortcut,
  shortcut: { modifiers: [], hidCode: 0x29 },
  holdModifiers: false,
  holdTimeMs: 0,
})

export const ShortcutWithSeveralModifiers = story({
  ...base,
  type: MacroDefinitionType.Shortcut,
  shortcut: { modifiers: [0xe0, 0xe1], hidCode: 0x29 },
  holdModifiers: false,
  holdTimeMs: 0,
})

// The modifiers stay held between repeats, for the hold time setting
export const ShortcutHoldingModifiers = story({
  ...base,
  type: MacroDefinitionType.Shortcut,
  shortcut: { modifiers: [0xe2], hidCode: 0x2b },
  holdModifiers: true,
  holdTimeMs: 0,
})

export const ShortcutHoldingModifiersForAWhile = story({
  ...base,
  type: MacroDefinitionType.Shortcut,
  shortcut: { modifiers: [0xe2], hidCode: 0x2b },
  holdModifiers: true,
  holdTimeMs: 800,
})

export const ConsumerControl = story({ ...base, type: MacroDefinitionType.ConsumerControl, usageId: 0xe9 })

export const SystemControl = story({ ...base, type: MacroDefinitionType.SystemControl, code: 0x02 })

export const Text = story({ ...base, type: MacroDefinitionType.Text, text: "Kind regards" })

// Cut off at the end of the line
export const LongText = story({
  ...base,
  type: MacroDefinitionType.Text,
  text: "Thanks for getting back to me so quickly. I'll have a look at the changes and get back to you tomorrow.",
})

export const MouseScroll = story({ ...base, type: MacroDefinitionType.MouseScroll, modifiers: [], wheel: 3, pan: 0 })

export const MouseScrollInBothDirections = story({
  ...base,
  type: MacroDefinitionType.MouseScroll,
  modifiers: [],
  wheel: -2,
  pan: 4,
})

export const MouseScrollWithModifiers = story({
  ...base,
  type: MacroDefinitionType.MouseScroll,
  modifiers: [0xe0],
  wheel: 1,
  pan: 0,
})

export const MouseButton = story({ ...base, type: MacroDefinitionType.MouseButton, modifiers: [], buttons: 0x04 })

export const MouseButtonsWithModifiers = story({
  ...base,
  type: MacroDefinitionType.MouseButton,
  modifiers: [0xe1],
  buttons: 0x01 | 0x02,
})

export const MouseMovement = story({ ...base, type: MacroDefinitionType.MouseMovement, modifiers: [], x: 10, y: -20 })

export const MouseMovementWithModifiers = story({
  ...base,
  type: MacroDefinitionType.MouseMovement,
  modifiers: [0xe2],
  x: -5,
  y: 0,
})

export const Sequence = story({
  ...base,
  type: MacroDefinitionType.Sequence,
  steps: [
    { macroId: 1, delayMs: 0 },
    { macroId: 5, delayMs: 0 },
  ],
})

// The delays are below the arrows to their steps, also before the first step
export const SequenceWithDelays = story({
  ...base,
  type: MacroDefinitionType.Sequence,
  steps: [
    { macroId: 1, delayMs: 250 },
    { macroId: 2, delayMs: 500 },
    { macroId: 5, delayMs: 2000 },
    { macroId: 3, delayMs: 1500 },
  ],
})

// Cut off, fading out towards the end
export const LongSequence = story({
  ...base,
  type: MacroDefinitionType.Sequence,
  steps: [
    { macroId: 1, delayMs: 0 },
    { macroId: 2, delayMs: 500 },
    { macroId: 5, delayMs: 0 },
    { macroId: 9, delayMs: 250 },
    { macroId: 10, delayMs: 0 },
    { macroId: 1, delayMs: 1000 },
    { macroId: 2, delayMs: 0 },
    { macroId: 5, delayMs: 500 },
  ],
})

// A step whose macro was deleted
export const SequenceWithMissingMacro = story({
  ...base,
  type: MacroDefinitionType.Sequence,
  steps: [
    { macroId: 1, delayMs: 0 },
    { macroId: 999, delayMs: 500 },
    { macroId: 5, delayMs: 0 },
  ],
})

// All the stored macros of the other stories, one below the other with their names, as in the macros pane
export const AllStoredMacros: Story = {
  args: { macro: macros[0] },
  render: () => (
    <Stack spacing={1.5}>
      {macros.map(it => (
        <Stack key={it.id} direction={"row"} spacing={2} alignItems={"center"}>
          <Typography variant={"body2"} sx={{ width: 120, flexShrink: 0 }} noWrap>{it.name}</Typography>
          <Box sx={{ width: PREVIEW_WIDTH, minWidth: 0, display: "flex" }}>
            <MacroDefinitionVisualization macro={it} />
          </Box>
        </Stack>
      ))}
    </Stack>
  ),
}
