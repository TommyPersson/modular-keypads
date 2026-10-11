import { mockKeyBindings, mockNotifications } from "@src/storybook"
import type { Meta, StoryObj } from "@storybook/react-vite"
import { devicePreviewCardMeta, macros, mockDeviceParameter, typeA } from "./DevicePreviewCard.storyData"

const meta = {
  ...devicePreviewCardMeta,
  title: "Device/Device Preview Card/Type A",
  args: {
    device: typeA,
  },
} satisfies Meta<typeof devicePreviewCardMeta.component>

export default meta

type Story = StoryObj<typeof meta>

export const Idle: Story = {
  parameters: mockDeviceParameter({ devices: [typeA] }),
}

export const WithPressedSwitches: Story = {
  parameters: mockDeviceParameter({
    devices: [typeA],
    notifications: [
      mockNotifications.pressed(typeA.deviceId, 1),
      mockNotifications.longPressed(typeA.deviceId, 6),
      mockNotifications.pressed(typeA.deviceId, 11),
      mockNotifications.pressed(typeA.deviceId, 12),
    ],
  }),
}

// Some switches with a press macro, some with a long press macro and some with both, some of them pressed
export const WithKeyBindings: Story = {
  parameters: mockDeviceParameter({
    devices: [typeA],
    macros,
    keyBindings: [
      mockKeyBindings.pressed(typeA.deviceId, 1, 1),
      mockKeyBindings.pressed(typeA.deviceId, 2, 2),
      mockKeyBindings.longPressed(typeA.deviceId, 2, 1),
      mockKeyBindings.pressed(typeA.deviceId, 5, 3),
      mockKeyBindings.longPressed(typeA.deviceId, 5, 4),
      mockKeyBindings.longPressed(typeA.deviceId, 6, 6),
      mockKeyBindings.pressed(typeA.deviceId, 9, 5),
    ],
    notifications: [
      mockNotifications.pressed(typeA.deviceId, 1),
      mockNotifications.longPressed(typeA.deviceId, 5),
    ],
  }),
}
