import { Box, Button, Stack } from "@mui/material"
import type { DeviceInformation } from "@src/modules/device/models"
import {
  mockDeviceParameter,
  smc1,
  smc2,
  smc3,
  typeA,
  typeC
} from "@src/modules/overview/ui/components/DevicePreviewCard/DevicePreviewCard.storyData"
import { mockDevice, mockNotifications, useMockDeviceFacade, withMockDevice } from "@src/storybook"
import type { Meta, StoryObj } from "@storybook/react-vite"
import { type ReactNode, useState } from "react"
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

const longNamedTypeC = mockDevice({
  deviceId: "c2000000000000c2",
  deviceType: "c",
  deviceName: "Right Hand Pad With A Name Far Too Long For The Card",
})

const longNamedSubDevice = (deviceId: string, deviceType: string, subDeviceSlot: number, deviceName: string) =>
  mockDevice({ deviceId, deviceType, deviceName, parentDeviceId: longNamedTypeC.deviceId, subDeviceSlot })

// Names of increasing length, from fitting to being cut off, for the devices and the sub-devices
export const LongNames: Story = {
  parameters: mockDeviceParameter({
    devices: [
      mockDevice({ deviceId: "a2000000000000a2", deviceType: "a", deviceName: "Left Hand Pad" }),
      longNamedTypeC,
      longNamedSubDevice("5c0000000000011", "smc1", 1, "Keys"),
      longNamedSubDevice("5c0000000000012", "smc2", 2, "Middle Row Of Six Keys"),
      longNamedSubDevice("5c0000000000013", "smc3", 3, "Volume Knob With A Name Far Too Long To Fit In The Caption Of The Preview"),
    ],
  }),
}

// Each top-level device with its sub-devices
const connectableDevices: DeviceInformation[][] = [
  [typeA],
  [typeC, smc1, smc2, smc3],
  [extraTypeA(1)],
]

const ConnectionToggles = () => {
  const facade = useMockDeviceFacade()

  const [connectedIds, setConnectedIds] = useState<string[]>([typeA.deviceId, typeC.deviceId])

  const toggle = (devices: DeviceInformation[]) => {
    const [device] = devices
    if (connectedIds.includes(device.deviceId)) {
      facade.disconnectDevice(device.deviceId)
      setConnectedIds(ids => ids.filter(it => it !== device.deviceId))
    } else {
      devices.forEach(it => facade.connectDevice(it))
      setConnectedIds(ids => [...ids, device.deviceId])
    }
  }

  return (
    <Stack direction={"row"} spacing={1}>
      {connectableDevices.map(devices => (
        <Button key={devices[0].deviceId} variant={"outlined"} onClick={() => toggle(devices)}>
          {connectedIds.includes(devices[0].deviceId) ? "Disconnect" : "Connect"} {devices[0].deviceName}
        </Button>
      ))}
    </Stack>
  )
}

// Connecting and disconnecting devices shifts the other cards to make room or close the gap
export const ConnectingAndDisconnecting: Story = {
  render: () => (
    <Stack spacing={2} sx={{ height: "100%", p: 2, boxSizing: "border-box" }}>
      <ConnectionToggles />
      <Box sx={{ flexGrow: 1, minHeight: 0 }}>
        <DevicePreviewList />
      </Box>
    </Stack>
  ),
  parameters: mockDeviceParameter({ devices: [typeA, typeC, smc1, smc2, smc3] }),
}

export const WithoutDevices: Story = {
  parameters: mockDeviceParameter({ devices: [] }),
}
