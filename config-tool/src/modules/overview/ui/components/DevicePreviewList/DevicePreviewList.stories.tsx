import { Box } from "@mui/material"
import {
  mockDeviceParameter,
  smc1,
  smc2,
  smc3,
  typeA,
  typeC
} from "@src/modules/device-debugger/ui/components/DevicePreviewCard/DevicePreviewCard.storyData"
import { mockDevice, mockNotifications, withMockDevice } from "@src/storybook"
import type { Meta, StoryObj } from "@storybook/react-vite"
import type { ReactNode } from "react"
import { DevicePreviewList } from "./DevicePreviewList"

// Outlined, to show the bounds of the parent container that the list fills
const ListContainer = (props: { width?: number, children: ReactNode }) => (
  <Box sx={{ width: props.width ?? "100%", height: "100%", outline: "1px dashed gray" }}>
    {props.children}
  </Box>
)

const meta = {
  title: "Overview/Device Preview List",
  component: DevicePreviewList,
  decorators: [
    (Story) => <ListContainer><Story /></ListContainer>,
    withMockDevice,
    (Story) => <Box sx={{ height: "100vh", p: 2, boxSizing: "border-box" }}><Story /></Box>,
  ],
  parameters: { layout: "fullscreen" },
} satisfies Meta<typeof DevicePreviewList>

export default meta

type Story = StoryObj<typeof meta>

// The order is kept in local storage, so moving cards in one story also affects the others
export const LeftAndRightPads: Story = {
  parameters: mockDeviceParameter({
    devices: [typeA, typeC, smc1, smc2, smc3],
    notifications: [
      mockNotifications.pressed(typeA.deviceId, 4),
      mockNotifications.rotatingClockwise(smc3.deviceId, 1),
    ],
  }),
}

export const SingleDevice: Story = {
  parameters: mockDeviceParameter({ devices: [typeC, smc1, smc2, smc3] }),
}

const extraTypeA = (index: number) =>
  mockDevice({ deviceId: `a10000000000000${index}`, deviceType: "a", deviceName: `Extra Pad ${index}` })

// In a narrow container, as in the overview page on a small screen, so that the list scrolls
export const Overflowing: Story = {
  decorators: [
    (Story) => <ListContainer width={700}><Story /></ListContainer>,
  ],
  parameters: mockDeviceParameter({ devices: [typeA, typeC, smc1, smc2, smc3, extraTypeA(1), extraTypeA(2)] }),
}

export const WithoutDevices: Story = {
  parameters: mockDeviceParameter({ devices: [] }),
}
