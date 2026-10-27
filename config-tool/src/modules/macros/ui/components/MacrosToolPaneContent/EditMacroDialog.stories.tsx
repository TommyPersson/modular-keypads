import { macros, typeC } from "@src/modules/overview/ui/components/DevicePreviewCard/DevicePreviewCard.storyData"
import { withMockDevice } from "@src/storybook"
import type { Meta, StoryObj } from "@storybook/react-vite"
import { fn } from "storybook/test"
import { EditMacroDialog } from "./EditMacroDialog"

const meta = {
  title: "Macros/Edit Macro Dialog",
  component: EditMacroDialog,
  decorators: [withMockDevice],
  args: {
    isOpen: true,
    onClose: fn(),
  },
  parameters: {
    mockDevice: {
      devices: [typeC],
      macros,
    },
  },
} satisfies Meta<typeof EditMacroDialog>

export default meta

type Story = StoryObj<typeof meta>

export const NewMacro: Story = {}

export const MouseScrollMacro: Story = {
  args: {
    macro: macros.find(it => it.name === "Scroll Up"),
  },
}

export const MouseMovementMacro: Story = {
  args: {
    macro: macros.find(it => it.name === "Nudge Right"),
  },
}

export const MidiNoteMacro: Story = {
  args: {
    macro: macros.find(it => it.name === "Kick Drum"),
  },
}

export const MidiRelativeControlChangeMacro: Story = {
  args: {
    macro: macros.find(it => it.name === "Mic Volume Up"),
  },
}

export const MidiAbsoluteControlChangeMacro: Story = {
  args: {
    macro: macros.find(it => it.name === "Desktop Volume Down"),
  },
}

export const RandomTextMacro: Story = {
  args: {
    macro: macros.find(it => it.name === "UUID"),
  },
}
