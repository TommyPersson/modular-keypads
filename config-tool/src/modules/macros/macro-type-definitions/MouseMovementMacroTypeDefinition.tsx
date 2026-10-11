import OpenWithOutlinedIcon from "@mui/icons-material/OpenWithOutlined"
import { Tooltip } from "@mui/material"
import {
  createModifierFlags,
  MacroDefinitionType,
  MacroTypeCategory,
  type MacroTypeDefinition,
  type MouseMovementMacroDefinition,
  parseModifierFlags
} from "@src/modules/macros/models"
import { MouseMovementMacroCellContent } from "./components/MouseMovementMacroCellContent"
import { MouseMovementMacroDefinitionEditor } from "./components/MouseMovementMacroDefinitionEditor"


export const MouseMovementMacroTypeDefinition: MacroTypeDefinition<MouseMovementMacroDefinition> = {
  type: MacroDefinitionType.MouseMovement,

  numericCode: 0x07,

  category: MacroTypeCategory.Mouse,

  displayName: "Mouse Movement",

  description: (
    <>
      Moves the <strong>mouse</strong> pointer relative to its current position.
    </>
  ),

  handles(type: MacroDefinitionType): boolean {
    return type === this.type
  },

  createDefaultMacroDefinition(name: string): MouseMovementMacroDefinition {
    return {
      id: 0,
      directory: null,
      name,
      type: MacroDefinitionType.MouseMovement,
      modifiers: [],
      x: 0,
      y: 0,
    }
  },

  parseDeviceResponse(id: number, directory: string | null, name: string, dataArgs: string[]): MouseMovementMacroDefinition {
    const [xArg, yArg, modifiersArg] = dataArgs

    return {
      id,
      directory,
      name,
      type: MacroDefinitionType.MouseMovement,
      modifiers: parseModifierFlags(parseInt(modifiersArg, 16) || 0),
      x: parseInt(xArg, 10) || 0,
      y: parseInt(yArg, 10) || 0,
    }
  },

  serializeDeviceArguments(macro: MouseMovementMacroDefinition): string[] {
    const modifiersArg = `0x${createModifierFlags(macro.modifiers).toString(16).padStart(2, "0")}`
    return ["0x07", `${macro.x}`, `${macro.y}`, modifiersArg]
  },

  editorInformationComponent: () => null,

  editorComponent: MouseMovementMacroDefinitionEditor,

  macroTableCellComponent: MouseMovementMacroCellContent,

  typeVisualizationComponent: () => (
    <Tooltip title={"Mouse Movement"}>
      <OpenWithOutlinedIcon fontSize={"small"} />
    </Tooltip>
  ),
}
