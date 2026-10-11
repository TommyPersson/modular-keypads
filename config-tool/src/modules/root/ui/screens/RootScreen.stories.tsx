import { ConfirmProvider } from "@src/modules/common/components"
import {
  mockDeviceParameter,
  smc1,
  smc2,
  smc3,
  typeA,
  typeC
} from "@src/modules/overview/ui/components/DevicePreviewCard/DevicePreviewCard.storyData"
import { OverviewPage } from "@src/modules/overview/ui/pages"
import { ToolPaneContextProvider } from "@src/modules/root/ui/components/ToolPane/ToolPaneContextProvider"
import { mockDevice, withMockDevice } from "@src/storybook"
import type { Meta, StoryObj } from "@storybook/react-vite"
import { ReactQueryDevtools } from "@tanstack/react-query-devtools"
import { useState } from "react"
import { createMemoryRouter, RouterProvider } from "react-router"
import { RootScreen } from "./RootScreen"

// The pages within the app's own layout, to see how they fit in it, e.g. next to the opened tool pane
const AppAt = (props: { path: string }) => {
  const [router] = useState(() => createMemoryRouter(
    [{ path: "/", Component: RootScreen, children: [{ path: "overview", Component: OverviewPage }] }],
    { initialEntries: [props.path] },
  ))

  return (
    <>

      <ReactQueryDevtools />
    <ConfirmProvider>
      <ToolPaneContextProvider>
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

export const Overview: Story = {
  args: { path: "/overview" },
  parameters: mockDeviceParameter({ devices: [typeA, typeC, smc1, smc2, smc3, extraTypeA] }),
}
