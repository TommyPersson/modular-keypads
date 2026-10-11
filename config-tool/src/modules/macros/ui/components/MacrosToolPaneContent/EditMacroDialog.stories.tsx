import { macros, typeC } from "@src/modules/device-debugger/ui/components/DevicePreviewCard/DevicePreviewCard.storyData"
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
