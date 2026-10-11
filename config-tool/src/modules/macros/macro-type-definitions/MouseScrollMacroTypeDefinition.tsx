import SwapVertOutlinedIcon from "@mui/icons-material/SwapVertOutlined"
import { Tooltip } from "@mui/material"
import {
  MacroDefinitionType,
  MacroTypeCategory,
  type MacroTypeDefinition,
  type MouseScrollMacroDefinition
} from "@src/modules/macros/models"
import { MouseScrollMacroCellContent } from "./components/MouseScrollMacroCellContent"
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
      wheel: 0,
      pan: 0,
    }
  },

  parseDeviceResponse(id: number, directory: string | null, name: string, dataArgs: string[]): MouseScrollMacroDefinition {
    const [wheelArg, panArg] = dataArgs

    return {
      id,
      directory,
      name,
      type: MacroDefinitionType.MouseScroll,
      wheel: parseInt(wheelArg, 10) || 0,
      pan: parseInt(panArg, 10) || 0,
    }
  },

  serializeDeviceArguments(macro: MouseScrollMacroDefinition): string[] {
    return ["0x05", `${macro.wheel}`, `${macro.pan}`]
  },

  editorInformationComponent: () => null,

  editorComponent: MouseScrollMacroDefinitionEditor,

  macroTableCellComponent: MouseScrollMacroCellContent,

  typeVisualizationComponent: () => (
    <Tooltip title={"Mouse Scroll"}>
      <SwapVertOutlinedIcon fontSize={"small"} />
    </Tooltip>
  ),
}
