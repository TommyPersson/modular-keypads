import ChevronLeftIcon from "@mui/icons-material/ChevronLeft"
import ChevronRightIcon from "@mui/icons-material/ChevronRight"
import { IconButton } from "@mui/material"
import { mockKeyBindings, type MockDeviceOptions, mockNotifications } from "@src/storybook"
import type { Meta, StoryObj } from "@storybook/react-vite"
import { DevicePreviewCard } from "./DevicePreviewCard"
import {
  capabilities,
  devicePreviewCardMeta,
  macros,
  mockDeviceParameter,
  smc1,
  smc2,
  smc3,
  typeC
} from "./DevicePreviewCard.storyData"

const meta = {
  ...devicePreviewCardMeta,
  title: "Device/Device Preview Card/Type C",
  args: {
    device: typeC,
  },
} satisfies Meta<typeof devicePreviewCardMeta.component>

export default meta

type Story = StoryObj<typeof meta>

export const WithAllSubDevices: Story = {
  parameters: mockDeviceParameter({ devices: [typeC, smc1, smc2, smc3] }),
}

export const WithActiveInputs: Story = {
  parameters: mockDeviceParameter({
    devices: [typeC, smc1, smc2, smc3],
    notifications: [
      mockNotifications.pressed(smc1.deviceId, 2),
      mockNotifications.longPressed(smc1.deviceId, 3),
      mockNotifications.pressed(smc2.deviceId, 1),
      mockNotifications.pressed(smc2.deviceId, 6),
      mockNotifications.pressed(smc3.deviceId, 1),
      mockNotifications.rotatingClockwise(smc3.deviceId, 1),
    ],
  }),
}

export const WithLongPressedEncoder: Story = {
  parameters: mockDeviceParameter({
    devices: [typeC, smc3],
    notifications: [
      mockNotifications.longPressed(smc3.deviceId, 1),
      mockNotifications.rotatingCounterClockwise(smc3.deviceId, 1),
    ],
  }),
}

// The 1u switches of the middle sub-device are narrower, so more of the names are cut off
const withKeyBindings: MockDeviceOptions = {
  devices: [typeC, smc1, smc2, smc3],
  macros,
  capabilities,
  keyBindings: [
    mockKeyBindings.pressed(smc1.deviceId, 1, 1),
    mockKeyBindings.longPressed(smc1.deviceId, 1, 2),
    mockKeyBindings.pressed(smc1.deviceId, 4, 5),
    mockKeyBindings.pressed(smc2.deviceId, 1, 3),
    mockKeyBindings.longPressed(smc2.deviceId, 1, 4),
    mockKeyBindings.pressed(smc2.deviceId, 2, 2),
    mockKeyBindings.longPressed(smc2.deviceId, 6, 6),
    mockKeyBindings.counterClockwise(smc3.deviceId, 1, 7),
    mockKeyBindings.clockwise(smc3.deviceId, 1, 3),
    mockKeyBindings.pressed(smc3.deviceId, 1, 4),
    mockKeyBindings.longPressed(smc3.deviceId, 1, 6),
  ],
  notifications: [
    mockNotifications.pressed(smc2.deviceId, 2),
  ],
}

export const WithKeyBindings: Story = {
  parameters: mockDeviceParameter(withKeyBindings),
}

// The boxes of the bound macros light up with the encoder
export const WithActiveEncoderBindings: Story = {
  parameters: mockDeviceParameter({
    ...withKeyBindings,
    notifications: [
      mockNotifications.longPressed(smc3.deviceId, 1),
      mockNotifications.rotatingClockwise(smc3.deviceId, 1),
    ],
  }),
}

export const WithEmptySlots: Story = {
  parameters: mockDeviceParameter({ devices: [typeC, smc2] }),
}

export const WithoutSubDevices: Story = {
  parameters: mockDeviceParameter({ devices: [typeC] }),
}

// As on the overview page, where the action moves the card
export const WithAction: Story = {
  args: {
    action: (
      <>
        <IconButton size={"small"}><ChevronLeftIcon /></IconButton>
        <IconButton size={"small"}><ChevronRightIcon /></IconButton>
      </>
    ),
  },
  parameters: WithAllSubDevices.parameters,
}

// The card for the device the tool is connected to, as on the device debugger page
export const ConnectedDevice: Story = {
  render: () => <DevicePreviewCard />,
  parameters: WithAllSubDevices.parameters,
}
