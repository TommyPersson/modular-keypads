import AdsClickOutlinedIcon from "@mui/icons-material/AdsClickOutlined"
import { Tooltip } from "@mui/material"
import {
  MacroDefinitionType,
  MacroTypeCategory,
  type MacroTypeDefinition,
  MouseButton,
  type MouseButtonMacroDefinition
} from "@src/modules/macros/models"
import { MouseButtonMacroCellContent } from "./components/MouseButtonMacroCellContent"
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
      buttons: MouseButton.Left,
    }
  },

  parseDeviceResponse(id: number, directory: string | null, name: string, dataArgs: string[]): MouseButtonMacroDefinition {
    const [buttonsArg] = dataArgs

    return {
      id,
      directory,
      name,
      type: MacroDefinitionType.MouseButton,
      buttons: parseInt(buttonsArg, 16) || 0,
    }
  },

  serializeDeviceArguments(macro: MouseButtonMacroDefinition): string[] {
    return ["0x06", `0x${macro.buttons.toString(16).padStart(2, "0")}`]
  },

  editorInformationComponent: () => null,

  editorComponent: MouseButtonMacroDefinitionEditor,

  macroTableCellComponent: MouseButtonMacroCellContent,

  typeVisualizationComponent: () => (
    <Tooltip title={"Mouse Button"}>
      <AdsClickOutlinedIcon fontSize={"small"} />
    </Tooltip>
  ),
}
