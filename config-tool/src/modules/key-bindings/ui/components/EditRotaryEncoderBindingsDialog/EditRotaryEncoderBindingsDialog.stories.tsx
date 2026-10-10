import {
  capabilities,
  macros,
  smc3,
  typeC
} from "@src/modules/device-debugger/ui/components/DevicePreviewCard/DevicePreviewCard.storyData"
import { mockKeyBindings, withMockDevice } from "@src/storybook"
import type { Meta, StoryObj } from "@storybook/react-vite"
import { fn } from "storybook/test"
import { EditRotaryEncoderBindingsDialog } from "./EditRotaryEncoderBindingsDialog"

const meta = {
  title: "Key Bindings/Edit Rotary Encoder Bindings Dialog",
  component: EditRotaryEncoderBindingsDialog,
  decorators: [withMockDevice],
  args: {
    isOpen: true,
    deviceId: smc3.deviceId,
    encoderNumber: 1,
    buttonNumber: 1,
    onClose: fn(),
  },
  parameters: {
    mockDevice: {
      devices: [typeC, smc3],
      macros,
      capabilities,
    },
  },
} satisfies Meta<typeof EditRotaryEncoderBindingsDialog>

export default meta

type Story = StoryObj<typeof meta>

export const WithBindings: Story = {
  parameters: {
    mockDevice: {
      ...meta.parameters.mockDevice,
      keyBindings: [
        mockKeyBindings.counterClockwise(smc3.deviceId, 1, 7),
        mockKeyBindings.clockwise(smc3.deviceId, 1, 3),
        mockKeyBindings.pressed(smc3.deviceId, 1, 4),
        mockKeyBindings.longPressed(smc3.deviceId, 1, 6),
      ],
    },
  },
}

export const WithoutBindings: Story = {}
