import SwapVertOutlinedIcon from "@mui/icons-material/SwapVertOutlined"
import { Tooltip } from "@mui/material"
import {
  createModifierFlags,
  DeviceFields,
  deviceHex,
  MacroDefinitionType,
  MacroTypeCategory,
  type MacroTypeDefinition,
  type MouseScrollMacroDefinition,
  parseModifierFlags,
  serializeDeviceFields
} from "@src/modules/macros/models"
import { MouseScrollMacroCellContent } from "./components/MouseScrollMacroCellContent"
import { modifiersLabel, mouseScrollLabel, texts } from "./components/macroLabels"
import { MouseScrollMacroDefinitionEditor } from "./components/MouseScrollMacroDefinitionEditor"


export const MouseScrollMacroTypeDefinition: MacroTypeDefinition<MouseScrollMacroDefinition> = {
  type: MacroDefinitionType.MouseScroll,

  numericCode: 0x05,

  category: MacroTypeCategory.Mouse,

  displayName: "Mouse Scroll",

  description: (
    <>
      Scrolls the <strong>mouse</strong> wheel vertically and/or horizontally. One step is one notch of the wheel.
    </>
  ),

  handles(type: MacroDefinitionType): boolean {
    return type === this.type
  },

  createDefaultMacroDefinition(name: string): MouseScrollMacroDefinition {
    return {
      id: 0,
      directory: null,
      name,
      type: MacroDefinitionType.MouseScroll,
      modifiers: [],
      wheel: 0,
      pan: 0,
    }
  },

  parseDeviceResponse(id: number, directory: string | null, name: string, dataArgs: string[]): MouseScrollMacroDefinition | null {
    const fields = DeviceFields.parse(dataArgs)
    if (!fields) {
      return null
    }

    return {
      id,
      directory,
      name,
      type: MacroDefinitionType.MouseScroll,
      modifiers: parseModifierFlags(fields.number("m", 0, 16)),
      wheel: fields.number("w", 0),
      pan: fields.number("p", 0),
    }
  },

  serializeDeviceArguments(macro: MouseScrollMacroDefinition): string[] {
    return ["0x05", ...serializeDeviceFields({
      m: deviceHex(createModifierFlags(macro.modifiers), 2),
      w: macro.wheel,
      p: macro.pan,
    })]
  },

  editorInformationComponent: () => null,

  editorComponent: MouseScrollMacroDefinitionEditor,

  macroTableCellComponent: MouseScrollMacroCellContent,

  searchTexts(macro: MouseScrollMacroDefinition): string[] {
    return texts(modifiersLabel(macro.modifiers), mouseScrollLabel(macro.wheel, macro.pan))
  },

  typeVisualizationComponent: () => (
    <Tooltip title={"Mouse Scroll"}>
      <SwapVertOutlinedIcon fontSize={"small"} />
    </Tooltip>
  ),
}
