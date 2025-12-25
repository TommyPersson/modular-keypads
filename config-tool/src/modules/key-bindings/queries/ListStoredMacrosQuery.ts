import type { DeviceFacade } from "@src/modules/device/facade"
import { type MacroDefinition, MacroDefinitionType, ModifierKey } from "@src/modules/key-bindings/models"
import { type UseQueryOptions } from "@tanstack/react-query"

export const ListStoredMacrosQuery: (deviceFacade: DeviceFacade) => UseQueryOptions<MacroDefinition[]> = (deviceFacade) => ({
  queryKey: ["device", "macros", "stored-macros"],
  queryFn: async (): Promise<MacroDefinition[]> => {
    return testMacros
  },
  enabled: deviceFacade.isConnected,
})


const testMacros: MacroDefinition[] = [
  {
    id: "My First Macro",
    type: MacroDefinitionType.Shortcut,
    shortcut: {
      modifiers: [ModifierKey.Control, ModifierKey.Shift],
      hidCode: 0x04 // A
    }
  },
  {
    id: "IntelliJ: Run",
    type: MacroDefinitionType.Shortcut,
    shortcut: {
      modifiers: [ModifierKey.Shift],
      hidCode: 0x43 // F10
    }
  },
  {
    id: "IntelliJ: Debug",
    type: MacroDefinitionType.Shortcut,
    shortcut: {
      modifiers: [ModifierKey.Shift],
      hidCode: 0x42 // F9
    }
  },
  {
    id: "IntelliJ: Stop",
    type: MacroDefinitionType.Shortcut,
    shortcut: {
      modifiers: [ModifierKey.Control],
      hidCode: 0x3b // F2
    }
  },
  {
    id: "Vim: Save & Exit",
    type: MacroDefinitionType.ShortcutSequence,
    shortcuts: [
      {
        modifiers: [],
        hidCode: 0x29 // Escape
      },
      {
        modifiers: [ModifierKey.Shift],
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
    id: "Vim: Force Exit",
    type: MacroDefinitionType.ShortcutSequence,
    shortcuts: [
      {
        modifiers: [],
        hidCode: 0x29 // Escape
      },
      {
        modifiers: [ModifierKey.Shift],
        hidCode: 0x33 // ; :
      },
      {
        modifiers: [],
        hidCode: 0x14 // Q
      },
      {
        modifiers: [ModifierKey.Shift],
        hidCode: 0x1e // 1 !
      },
      {
        modifiers: [],
        hidCode: 0x28 // Enter
      }
    ]
  },
  {
    id: "Raw HID Sequence",
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