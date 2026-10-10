import { mockDevice, type MockDeviceOptions, withMockDevice } from "@src/storybook"
import type { Meta } from "@storybook/react-vite"
import { TopLevelDevicePreviewCard } from "./DevicePreviewCard"

// Shared by the device preview card stories, which are split by device type

export const typeA = mockDevice({ deviceId: "a1000000000000a1", deviceType: "a", deviceName: "Left Pad" })
export const typeC = mockDevice({ deviceId: "c1000000000000c1", deviceType: "c", deviceName: "Right Pad" })

export const subDeviceOfTypeC = (deviceId: string, deviceType: string, subDeviceSlot: number) =>
  mockDevice({ deviceId, deviceType, parentDeviceId: typeC.deviceId, subDeviceSlot })

export const smc1 = subDeviceOfTypeC("5c0000000000001", "smc1", 1)
export const smc2 = subDeviceOfTypeC("5c0000000000002", "smc2", 2)
export const smc3 = subDeviceOfTypeC("5c0000000000003", "smc3", 3)

export const mockDeviceParameter = (options: MockDeviceOptions) => ({ mockDevice: options })

export const devicePreviewCardMeta = {
  component: TopLevelDevicePreviewCard,
  decorators: [withMockDevice],
  // Centered, so that the card gets its natural width, as in the rows of cards in the app
  parameters: { layout: "centered" },
} satisfies Partial<Meta<typeof TopLevelDevicePreviewCard>>
