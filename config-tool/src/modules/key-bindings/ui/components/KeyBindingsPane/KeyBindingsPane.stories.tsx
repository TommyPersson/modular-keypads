import {
  capabilities,
  macros,
  smc1,
  smc3,
  typeC
} from "@src/modules/overview/ui/components/DevicePreviewCard/DevicePreviewCard.storyData"
import { mockKeyBindings, withKeyBindingsPane, withMockDevice } from "@src/storybook"
import type { Meta, StoryObj } from "@storybook/react-vite"
import type { KeyBindingTarget } from "./KeyBindingSelectionContext"

// The pane is shown by the decorator, which places it as on the overview page, with nothing above it
const meta = {
  title: "Key Bindings/Key Bindings Pane",
  render: () => <></>,
  decorators: [withKeyBindingsPane, withMockDevice],
  parameters: { layout: "centered" },
} satisfies Meta

export default meta

type Story = StoryObj<typeof meta>

const selected = (target: KeyBindingTarget) => ({ keyBindingsPane: { selectedTarget: target } })

const device = { devices: [typeC, smc1, smc3], macros, capabilities }

export const NothingSelected: Story = {
  parameters: { mockDevice: device },
}

export const PushButtonWithBindings: Story = {
  parameters: {
    ...selected({ type: "PushButton", deviceId: smc1.deviceId, buttonNumber: 1 }),
    mockDevice: {
      ...device,
      keyBindings: [
        mockKeyBindings.pressed(smc1.deviceId, 1, 1),
        mockKeyBindings.longPressed(smc1.deviceId, 1, 2),
      ],
    },
  },
}

export const PushButtonWithoutBindings: Story = {
  parameters: {
    ...selected({ type: "PushButton", deviceId: smc1.deviceId, buttonNumber: 2 }),
    mockDevice: device,
  },
}

// The button of the encoder has no identification light, so the flash button is disabled
export const RotaryEncoderWithBindings: Story = {
  parameters: {
    ...selected({ type: "RotaryEncoder", deviceId: smc3.deviceId, encoderNumber: 1, buttonNumber: 1 }),
    mockDevice: {
      ...device,
      keyBindings: [
        mockKeyBindings.counterClockwise(smc3.deviceId, 1, 7),
        mockKeyBindings.clockwise(smc3.deviceId, 1, 3),
        mockKeyBindings.pressed(smc3.deviceId, 1, 4),
        mockKeyBindings.longPressed(smc3.deviceId, 1, 6),
      ],
    },
  },
}

export const RotaryEncoderWithoutBindings: Story = {
  parameters: {
    ...selected({ type: "RotaryEncoder", deviceId: smc3.deviceId, encoderNumber: 1, buttonNumber: 1 }),
    mockDevice: device,
  },
}
