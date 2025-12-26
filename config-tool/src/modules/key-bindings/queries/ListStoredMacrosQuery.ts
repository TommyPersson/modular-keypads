import type { DeviceFacade } from "@src/modules/device/facade"
import { type MacroDefinition, MacroDefinitionType } from "@src/modules/key-bindings/models"
import { type UseQueryOptions } from "@tanstack/react-query"

export const ListStoredMacrosQueryKey = ["device", "macros", "stored-macros"]

export const ListStoredMacrosQuery: (deviceFacade: DeviceFacade) => UseQueryOptions<MacroDefinition[]> = (deviceFacade) => ({
  queryKey: ListStoredMacrosQueryKey,
  queryFn: async (): Promise<MacroDefinition[]> => {
    return testMacros
  },
  enabled: deviceFacade.isConnected,
})

const testMacros: MacroDefinition[] = [
  {
    id: 1,
    name: "My First Macro",
    type: MacroDefinitionType.Shortcut,
    shortcut: {
      modifiers: [0xe0, 0xe1], // Control (L), Shift (L)
      hidCode: 0x04 // A
    }
  },
  {
    id: 2,
    name: "IntelliJ: Run",
    type: MacroDefinitionType.Shortcut,
    shortcut: {
      modifiers: [0xe1], // Shift (L)
      hidCode: 0x43 // F10
    }
  },
  {
    id: 3,
    name: "IntelliJ: Debug",
    type: MacroDefinitionType.Shortcut,
    shortcut: {
      modifiers: [0xe1], // Shift (L)
      hidCode: 0x42 // F9
    }
  },
  {
    id: 4,
    name: "IntelliJ: Stop",
    type: MacroDefinitionType.Shortcut,
    shortcut: {
      modifiers: [0xe0], // Control (L)
      hidCode: 0x3b // F2
    }
  },
  {
    id: 5,
    name: "Vim: Save & Exit",
    type: MacroDefinitionType.ShortcutSequence,
    shortcuts: [
      {
        modifiers: [],
        hidCode: 0x29 // Escape
      },
      {
        modifiers: [0xe1], // Shift (L)
        hidCode: 0x33 // ; :
      },
      {
        modifiers: [],
        hidCode: 0x1a // W
      },
      {
        modifiers: [],
        hidCode: 0x14 // Q
      },
      {
        modifiers: [],
        hidCode: 0x28 // Enter
      }
    ]
  },
  {
    id: 6,
    name: "Vim: Force Exit",
    type: MacroDefinitionType.ShortcutSequence,
    shortcuts: [
      {
        modifiers: [],
        hidCode: 0x29 // Escape
      },
      {
        modifiers: [0xe1], // Shift (L)
        hidCode: 0x33 // ; :
      },
      {
        modifiers: [],
        hidCode: 0x14 // Q
      },
      {
        modifiers: [0xe1], // Shift (L)
        hidCode: 0x1e // 1 !
      },
      {
        modifiers: [],
        hidCode: 0x28 // Enter
      }
    ]
  },
  {
    id: 7,
    name: "Raw HID Sequence",
    type: MacroDefinitionType.HIDKeySequence,
    sequence: [
      {
        type: "Press",
        hidCode: 0x29
      },
      {
        type: "Press",
        hidCode: 0x30
      },
      {
        type: "Release",
        hidCode: 0x30
      },
      {
        type: "Release",
        hidCode: 0x29
      }
    ]
  }
]