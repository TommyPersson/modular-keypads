import SwapVertOutlinedIcon from "@mui/icons-material/SwapVertOutlined"
import { Tooltip } from "@mui/material"
import {
  createModifierFlags,
  MacroDefinitionType,
  MacroTypeCategory,
  type MacroTypeDefinition,
  type MouseScrollMacroDefinition,
  parseModifierFlags
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

  parseDeviceResponse(id: number, directory: string | null, name: string, dataArgs: string[]): MouseScrollMacroDefinition {
    const [modifiersArg, wheelArg, panArg] = dataArgs

    return {
      id,
      directory,
      name,
      type: MacroDefinitionType.MouseScroll,
      modifiers: parseModifierFlags(parseInt(modifiersArg, 16)),
      wheel: parseInt(wheelArg, 10),
      pan: parseInt(panArg, 10),
    }
  },

  serializeDeviceArguments(macro: MouseScrollMacroDefinition): string[] {
    const modifiersArg = `0x${createModifierFlags(macro.modifiers).toString(16).padStart(2, "0")}`
    return ["0x05", modifiersArg, `${macro.wheel}`, `${macro.pan}`]
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
