import KeyboardCommandKeyOutlinedIcon from "@mui/icons-material/KeyboardCommandKeyOutlined"
import { Alert, Tooltip } from "@mui/material"
import {
  createModifierFlags,
  DeviceFields,
  deviceHex,
  MacroDefinitionType,
  MacroTypeCategory,
  type MacroTypeDefinition,
  parseModifierFlags,
  serializeDeviceFields,
  type ShortcutMacroDefinition
} from "@src/modules/macros/models"
import { ShortcutMacroCellContent } from "./components/ShortcutMacroCellContent"
import { keyCodeLabel, modifiersLabel, shortcutHoldLabel, texts } from "./components/macroLabels"
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

  parseDeviceResponse(id: number, directory: string | null, name: string, dataArgs: string[]): ShortcutMacroDefinition | null {
    const fields = DeviceFields.parse(dataArgs)
    if (!fields) {
      return null
    }

    return {
      id,
      directory,
      name,
      type: MacroDefinitionType.Shortcut,
      shortcut: {
        modifiers: parseModifierFlags(fields.number("m", 0, 16)),
        hidCode: fields.number("k", 0, 16),
      },
      holdModifiers: fields.flag("h"),
      holdTimeMs: fields.number("t", 0),
    } satisfies ShortcutMacroDefinition
  },

  serializeDeviceArguments(macro: ShortcutMacroDefinition): string[] {
    return ["0x01", ...serializeDeviceFields({
      m: deviceHex(createModifierFlags(macro.shortcut.modifiers), 2),
      k: deviceHex(macro.shortcut.hidCode, 2),
      h: macro.holdModifiers,
      t: macro.holdTimeMs,
    })]
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

  searchTexts(macro: ShortcutMacroDefinition): string[] {
    return texts(
      modifiersLabel(macro.shortcut.modifiers),
      ...Object.values(keyCodeLabel(macro.shortcut.hidCode)),
      shortcutHoldLabel(macro),
    )
  },

  typeVisualizationComponent: () => (
    <Tooltip title={"Shortcut"}>
      <KeyboardCommandKeyOutlinedIcon fontSize={"small"} />
    </Tooltip>
  ),
}

