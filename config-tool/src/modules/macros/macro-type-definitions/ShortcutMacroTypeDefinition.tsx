import KeyboardCommandKeyOutlinedIcon from "@mui/icons-material/KeyboardCommandKeyOutlined"
import { Alert, Tooltip } from "@mui/material"
import {
  createModifierFlags,
  MacroDefinitionType,
  MacroTypeCategory,
  type MacroTypeDefinition,
  parseModifierFlags,
  type ShortcutMacroDefinition
} from "@src/modules/macros/models"
import { ShortcutMacroCellContent } from "./components/ShortcutMacroCellContent"
import { ShortcutMacroDefinitionEditor } from "./components/ShortcutMacroDefinitionEditor"


export const ShortcutMacroTypeDefinition: MacroTypeDefinition<ShortcutMacroDefinition> = {
  type: MacroDefinitionType.Shortcut,

  numericCode: 0x01,

  category: MacroTypeCategory.Keyboard,

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
      directory: null,
      name: name,
      type: MacroDefinitionType.Shortcut,
      shortcut: {
        modifiers: [],
        hidCode: 0
      },
      holdModifiers: false,
      holdTimeMs: 0,
    }
  },

  parseDeviceResponse(id: number, directory: string | null, name: string, dataArgs: string[]): ShortcutMacroDefinition {
    const [modifiersStr, hidCodeStr, holdModifiersStr, holdTimeMsStr] = dataArgs
    const modifiers = parseModifierFlags(parseInt(modifiersStr, 16))
    const hidCode = parseInt(hidCodeStr, 16)

    return {
      id,
      directory,
      name,
      type: MacroDefinitionType.Shortcut,
      shortcut: {
        modifiers,
        hidCode
      },
      holdModifiers: holdModifiersStr === "1",
      holdTimeMs: parseInt(holdTimeMsStr, 10),
    } satisfies ShortcutMacroDefinition
  },

  serializeDeviceArguments(macro: ShortcutMacroDefinition): string[] {
    const modifiersArg = `0x${createModifierFlags(macro.shortcut.modifiers).toString(16).padStart(2, "0")}`
    const hidCodeArg = `0x${macro.shortcut.hidCode.toString(16).padStart(2, "0")}`
    return ["0x01", modifiersArg, hidCodeArg, macro.holdModifiers ? "1" : "0", `${macro.holdTimeMs}`]
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

