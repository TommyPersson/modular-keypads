import AdsClickOutlinedIcon from "@mui/icons-material/AdsClickOutlined"
import { Tooltip } from "@mui/material"
import {
  createModifierFlags,
  DeviceFields,
  deviceHex,
  MacroDefinitionType,
  MacroTypeCategory,
  type MacroTypeDefinition,
  MouseButton,
  type MouseButtonMacroDefinition,
  parseModifierFlags,
  serializeDeviceFields
} from "@src/modules/macros/models"
import { MouseButtonMacroCellContent } from "./components/MouseButtonMacroCellContent"
import { modifiersLabel, mouseButtonLabel, texts } from "./components/macroLabels"
import { MouseButtonMacroDefinitionEditor } from "./components/MouseButtonMacroDefinitionEditor"


export const MouseButtonMacroTypeDefinition: MacroTypeDefinition<MouseButtonMacroDefinition> = {
  type: MacroDefinitionType.MouseButton,

  numericCode: 0x06,

  category: MacroTypeCategory.Mouse,

  displayName: "Mouse Button",

  description: (
    <>
      Clicks one or more <strong>mouse</strong> buttons.
    </>
  ),

  handles(type: MacroDefinitionType): boolean {
    return type === this.type
  },

  createDefaultMacroDefinition(name: string): MouseButtonMacroDefinition {
    return {
      id: 0,
      directory: null,
      name,
      type: MacroDefinitionType.MouseButton,
      modifiers: [],
      buttons: MouseButton.Left,
    }
  },

  parseDeviceResponse(id: number, directory: string | null, name: string, dataArgs: string[]): MouseButtonMacroDefinition | null {
    const fields = DeviceFields.parse(dataArgs)
    if (!fields) {
      return null
    }

    return {
      id,
      directory,
      name,
      type: MacroDefinitionType.MouseButton,
      modifiers: parseModifierFlags(fields.number("m", 0, 16)),
      buttons: fields.number("b", 0, 16),
    }
  },

  serializeDeviceArguments(macro: MouseButtonMacroDefinition): string[] {
    return ["0x06", ...serializeDeviceFields({
      m: deviceHex(createModifierFlags(macro.modifiers), 2),
      b: deviceHex(macro.buttons, 2),
    })]
  },

  editorInformationComponent: () => null,

  editorComponent: MouseButtonMacroDefinitionEditor,

  macroTableCellComponent: MouseButtonMacroCellContent,

  searchTexts(macro: MouseButtonMacroDefinition): string[] {
    return texts(modifiersLabel(macro.modifiers), mouseButtonLabel(macro.buttons))
  },

  typeVisualizationComponent: () => (
    <Tooltip title={"Mouse Button"}>
      <AdsClickOutlinedIcon fontSize={"small"} />
    </Tooltip>
  ),
}
