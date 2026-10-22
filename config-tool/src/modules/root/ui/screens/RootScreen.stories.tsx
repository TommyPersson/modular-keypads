import { ConfirmProvider } from "@src/modules/common/components"
import {
  macros,
  mockDeviceParameter,
  smc1,
  smc2,
  smc3,
  typeA,
  typeC
} from "@src/modules/overview/ui/components/DevicePreviewCard/DevicePreviewCard.storyData"
import { OverviewPage } from "@src/modules/overview/ui/pages"
import { ToolPaneContextProvider } from "@src/modules/root/ui/components/ToolPane/ToolPaneContextProvider"
import { SettingType } from "@src/modules/settings/models"
import { mockDevice, type MockDeviceOptions, mockNotifications, withMockDevice } from "@src/storybook"
import type { Meta, StoryObj } from "@storybook/react-vite"
import { ReactQueryDevtools } from "@tanstack/react-query-devtools"
import { useState } from "react"
import { createMemoryRouter, RouterProvider } from "react-router"
import { RootScreen } from "./RootScreen"

// The pages within the app's own layout, to see how they fit in it, e.g. next to the opened tool pane
const AppAt = (props: { path: string, toolId?: string }) => {
  const [router] = useState(() => createMemoryRouter(
    [{ path: "/", Component: RootScreen, children: [{ path: "overview", Component: OverviewPage }] }],
    { initialEntries: [props.path] },
  ))

  return (
    <>
      <ReactQueryDevtools />
      <ConfirmProvider>
        <ToolPaneContextProvider initialSelectedToolId={props.toolId}>
          <RouterProvider router={router} />
        </ToolPaneContextProvider>
      </ConfirmProvider>
    </>
  )
}

const meta = {
  title: "App/Root Screen",
  component: AppAt,
  decorators: [withMockDevice],
  parameters: { layout: "fullscreen" },
} satisfies Meta<typeof AppAt>

export default meta

type Story = StoryObj<typeof meta>

const extraTypeA = mockDevice({ deviceId: "a3000000000000a3", deviceType: "a", deviceName: "Extra Pad" })

const device: MockDeviceOptions = {
  devices: [typeA, typeC, smc1, smc2, smc3, extraTypeA],
  macros,
  settings: [
    { key: "typing.delay", type: SettingType.UInt, value: 10, defaultValue: 5, minValue: 0, maxValue: 1000 },
    { key: "modifier.hold", type: SettingType.UInt, value: 500, defaultValue: 500, minValue: 0, maxValue: 5000 },
  ],
  metrics: [
    { name: "firmware.master.device_loop_time_us", value: 412 },
    { name: "esp.memory.global.free_heap_size", value: 214332 },
    { name: "esp.memory.global.minimum_free_heap_size", value: 198104 },
    { name: "keybindings.num_assigned", value: 12 },
    { name: "macros.num_stored", value: macros.length },
    { name: "littlefs.used_bytes", value: 24576 },
    { name: "littlefs.total_bytes", value: 1441792 },
  ],
  logs: [
    { level: "info", component: "discovery", message: "Found 5 devices" },
    { level: "debug", component: "i2c", message: "Assigned address 0x21 to c1000000000000c1" },
    { level: "warning", component: "macros", message: "Typing is slower than the typing delay" },
    { level: "error", component: "i2c", message: "No response from 0x23, retrying" },
  ],
}

export const Overview: Story = {
  args: { path: "/overview" },
  parameters: mockDeviceParameter(device),
}

// Switch 5 of the middle keys is pressed every two seconds, which selects it once test mode is turned on
export const PressingInTestMode: Story = {
  args: { path: "/overview" },
  parameters: mockDeviceParameter({
    ...device,
    notifications: [{ ...mockNotifications.pressed(smc2.deviceId, 5), repeatMs: 2000 }],
  }),
}

export const Disconnected: Story = {
  args: { path: "/overview" },
  parameters: mockDeviceParameter({ ...device, connected: false }),
}

export const KeyboardEventsPane: Story = {
  args: { path: "/overview", toolId: "keyboard-events" },
  parameters: mockDeviceParameter(device),
}

export const MacrosPane: Story = {
  args: { path: "/overview", toolId: "macros" },
  parameters: mockDeviceParameter(device),
}

export const SettingsPane: Story = {
  args: { path: "/overview", toolId: "settings" },
  parameters: mockDeviceParameter(device),
}

export const DeviceLogsPane: Story = {
  args: { path: "/overview", toolId: "device-logs" },
  parameters: mockDeviceParameter(device),
}

export const DeviceMetricsPane: Story = {
  args: { path: "/overview", toolId: "device-metrics" },
  parameters: mockDeviceParameter(device),
}

export const DeviceTrafficPane: Story = {
  args: { path: "/overview", toolId: "device-traffic" },
  parameters: mockDeviceParameter(device),
}
