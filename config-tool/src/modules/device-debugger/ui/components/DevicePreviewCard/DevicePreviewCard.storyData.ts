import { mockDevice, type MockDeviceOptions, withMockDevice } from "@src/storybook"
import { type MacroDefinition, MacroDefinitionType } from "@src/modules/macros/models"
import type { Meta } from "@storybook/react-vite"
import { TopLevelDevicePreviewCard } from "./DevicePreviewCard"

// Shared by the device preview card stories, which are split by device type

export const typeA = mockDevice({ deviceId: "a1000000000000a1", deviceType: "a", deviceName: "Left Pad" })
export const typeC = mockDevice({ deviceId: "c1000000000000c1", deviceType: "c", deviceName: "Right Pad" })

export const subDeviceOfTypeC = (deviceId: string, deviceType: string, subDeviceSlot: number, deviceName: string) =>
  mockDevice({ deviceId, deviceType, deviceName, parentDeviceId: typeC.deviceId, subDeviceSlot })

export const smc1 = subDeviceOfTypeC("5c0000000000001", "smc1", 1, "Top Keys")
export const smc2 = subDeviceOfTypeC("5c0000000000002", "smc2", 2, "Middle Keys")
export const smc3 = subDeviceOfTypeC("5c0000000000003", "smc3", 3, "Knob")

// Of all types, with names both short and too long to fit on the switches
export const macros: MacroDefinition[] = [
  { id: 1, directory: null, name: "Hello", type: MacroDefinitionType.Text, text: "Hello" },
  { id: 2, directory: "Signatures", name: "Kind regards", type: MacroDefinitionType.Text, text: "Kind regards" },
  { id: 3, directory: "Media", name: "Volume Up", type: MacroDefinitionType.ConsumerControl, usageId: 0xe9 },
  { id: 7, directory: "Media", name: "Volume Down", type: MacroDefinitionType.ConsumerControl, usageId: 0xea },
  { id: 4, directory: "Media", name: "Mute", type: MacroDefinitionType.ConsumerControl, usageId: 0xe2 },
  {
    id: 5,
    directory: null,
    name: "Copy",
    type: MacroDefinitionType.Shortcut,
    shortcut: { modifiers: [0xe0], hidCode: 0x06 },
  },
  { id: 6, directory: null, name: "Sleep", type: MacroDefinitionType.SystemControl, code: 0x82 },
]

export const mockDeviceParameter =(options: MockDeviceOptions) => ({ mockDevice: options })

export const devicePreviewCardMeta = {
  component: TopLevelDevicePreviewCard,
  decorators: [withMockDevice],
  // Centered, so that the card gets its natural width, as in the rows of cards in the app
  parameters: { layout: "centered" },
} satisfies Partial<Meta<typeof TopLevelDevicePreviewCard>>
