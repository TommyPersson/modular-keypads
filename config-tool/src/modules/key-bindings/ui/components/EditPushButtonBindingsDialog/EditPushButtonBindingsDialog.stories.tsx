import {
  capabilities,
  macros,
  smc1,
  typeC
} from "@src/modules/device-debugger/ui/components/DevicePreviewCard/DevicePreviewCard.storyData"
import { mockKeyBindings, withMockDevice } from "@src/storybook"
import type { Meta, StoryObj } from "@storybook/react-vite"
import { fn } from "storybook/test"
import { EditPushButtonBindingsDialog } from "./EditPushButtonBindingsDialog"

const meta = {
  title: "Key Bindings/Edit Push Button Bindings Dialog",
  component: EditPushButtonBindingsDialog,
  decorators: [withMockDevice],
  args: {
    isOpen: true,
    deviceId: smc1.deviceId,
    buttonNumber: 1,
    onClose: fn(),
  },
  parameters: {
    mockDevice: {
      devices: [typeC, smc1],
      macros,
      capabilities,
      keyBindings: [
        mockKeyBindings.pressed(smc1.deviceId, 1, 1),
        mockKeyBindings.longPressed(smc1.deviceId, 1, 2),
      ],
    },
  },
} satisfies Meta<typeof EditPushButtonBindingsDialog>

export default meta

type Story = StoryObj<typeof meta>

export const WithBindings: Story = {}

export const WithoutBindings: Story = {
  args: {
    buttonNumber: 2,
  },
}
