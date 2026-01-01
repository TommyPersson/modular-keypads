import KeyboardCommandKeyOutlinedIcon from "@mui/icons-material/KeyboardCommandKeyOutlined"
import { Alert, Tooltip } from "@mui/material"
import {
  createModifierFlags,
  MacroDefinitionType, parseModifierFlags,
  type ShortcutMacroDefinition
} from "@src/modules/key-bindings/models"
import { ShortcutMacroCellContent } from "@src/modules/macro-types/components/ShortcutMacroCellContent"
import { ShortcutMacroDefinitionEditor } from "@src/modules/macro-types/components/ShortcutMacroDefinitionEditor"
import type { MacroTypeDefinition } from "@src/modules/macro-types/MacroTypeDefinition"


export const ShortcutMacroTypeDefinition: MacroTypeDefinition<ShortcutMacroDefinition> = {
  type: MacroDefinitionType.Shortcut,

  numericCode: 0x01,

  displayName: "Shortcut",

  description: (
    <>
      A <strong>Shortcut</strong> is a single regular key press, possibly combined with
      modifiers.
    </>
  ),

  handles(type: MacroDefinitionType): boolean {
    return type === this.type
  },

  createDefaultMacroDefinition(name: string): ShortcutMacroDefinition {
    return {
      id: 0,
      name: name,
      type: MacroDefinitionType.Shortcut,
      shortcut: {
        modifiers: [],
        hidCode: 0
      },
    }
  },

  parseDeviceResponse(id: number, name: string, dataArgs: string[]): ShortcutMacroDefinition {
    const [modifiersStr, hidCodeStr] = dataArgs
    const modifiers = parseModifierFlags(parseInt(modifiersStr, 16))
    const hidCode = parseInt(hidCodeStr, 16)

    return {
      id,
      name,
      type: MacroDefinitionType.Shortcut,
      shortcut: {
        modifiers,
        hidCode
      }
    } satisfies ShortcutMacroDefinition
  },

  serializeDeviceArguments(macro: ShortcutMacroDefinition): string[] {
    const modifiersArg = `0x${createModifierFlags(macro.shortcut.modifiers).toString(16).padStart(2, "0")}`
    const hidCodeArg = `0x${macro.shortcut.hidCode.toString(16).padStart(2, "0")}`
    return ["0x01", modifiersArg, hidCodeArg]
  },

  editorInformationComponent: () => (
    <Alert severity={"info"}>
      Keep in mind that the keys shown in HID code list are the raw HID descriptions. The values has not been
      updated to match the current locale. For example, the <code>Q</code> key should be selected instead of
      <code>A</code> in AZERTY layouts.
    </Alert>
  ),

  editorComponent: ShortcutMacroDefinitionEditor,

  macroTableCellComponent: ShortcutMacroCellContent,

  typeVisualizationComponent: () => (
    <Tooltip title={"Shortcut"}>
      <KeyboardCommandKeyOutlinedIcon fontSize={"small"} />
    </Tooltip>
  ),
}

