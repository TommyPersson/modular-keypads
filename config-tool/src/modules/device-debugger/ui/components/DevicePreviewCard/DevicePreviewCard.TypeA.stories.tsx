import { mockNotifications } from "@src/storybook"
import type { Meta, StoryObj } from "@storybook/react-vite"
import { devicePreviewCardMeta, mockDeviceParameter, typeA } from "./DevicePreviewCard.storyData"

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
