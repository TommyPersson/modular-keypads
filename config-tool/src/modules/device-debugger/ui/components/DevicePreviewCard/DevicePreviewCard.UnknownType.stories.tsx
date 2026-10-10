import { mockDevice } from "@src/storybook"
import type { Meta, StoryObj } from "@storybook/react-vite"
import {
  devicePreviewCardMeta,
  mockDeviceParameter,
  smc1,
  smc3,
  subDeviceOfTypeC,
  typeC
} from "./DevicePreviewCard.storyData"

const unknownDevice = mockDevice({ deviceId: "f1000000000000f1", deviceType: "x", deviceName: "Mystery Pad" })

const meta = {
  ...devicePreviewCardMeta,
  title: "Device/Device Preview Card/Unknown Type",
  args: {
    title: unknownDevice.deviceName,
    deviceId: unknownDevice.deviceId,
    deviceType: unknownDevice.deviceType,
  },
} satisfies Meta<typeof devicePreviewCardMeta.component>

export default meta

type Story = StoryObj<typeof meta>

export const UnknownDeviceType: Story = {
  parameters: mockDeviceParameter({ devices: [unknownDevice] }),
}

// A sub-device of an unknown type, in slot 2 of a type C device
export const UnknownSubDeviceType: Story = {
  args: {
    title: typeC.deviceName,
    deviceId: typeC.deviceId,
    deviceType: typeC.deviceType,
  },
  parameters: mockDeviceParameter({
    devices: [typeC, smc1, subDeviceOfTypeC("5c0000000000004", "smc9", 2), smc3],
  }),
}
