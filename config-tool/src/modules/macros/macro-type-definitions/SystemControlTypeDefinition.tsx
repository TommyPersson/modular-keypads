import ComputerOutlinedIcon from "@mui/icons-material/ComputerOutlined"
import { Tooltip } from "@mui/material"
import {
  MacroDefinitionType,
  MacroTypeCategory,
  type MacroTypeDefinition,
  type SystemControlMacroDefinition
} from "@src/modules/macros/models"
import { SystemControlMacroCellContent } from "./components/SystemControlMacroCellContent"
import { systemControlLabel, texts } from "./components/macroLabels"
import { SystemControlMacroDefinitionEditor } from "./components/SystemControlMacroDefinitionEditor"


export const SystemControlTypeDefinition: MacroTypeDefinition<SystemControlMacroDefinition> = {
  type: MacroDefinitionType.SystemControl,

  numericCode: 0x03,

  category: MacroTypeCategory.Keyboard,

  displayName: "System Control",

  description: (
    <>
      The <strong>System Control</strong> specification lists a few system power related controls.
    </>
  ),

  handles(type: MacroDefinitionType): boolean {
    return type === this.type
  },

  createDefaultMacroDefinition(name: string): SystemControlMacroDefinition {
    return {
      id: 0,
      directory: null,
      name,
      type: MacroDefinitionType.SystemControl,
      code: 0
    }
  },

  parseDeviceResponse(id: number, directory: string | null, name: string, dataArgs: string[]): SystemControlMacroDefinition {
    const codePart = dataArgs[0]
    const code = parseInt(codePart, 16)

    return {
      id,
      directory,
      name,
      type: MacroDefinitionType.SystemControl,
      code: code
    }
  },

  serializeDeviceArguments(macro: SystemControlMacroDefinition): string[] {
    const usageIdArg = `0x${macro.code.toString(16).padStart(2, "0")}`
    return ["0x03", usageIdArg]
  },

  editorInformationComponent: () => null,

  editorComponent: SystemControlMacroDefinitionEditor,

  macroTableCellComponent: SystemControlMacroCellContent,

  searchTexts(macro: SystemControlMacroDefinition): string[] {
    return texts(systemControlLabel(macro.code))
  },

  typeVisualizationComponent: () => (
    <Tooltip title={"System Control"}>
      <ComputerOutlinedIcon fontSize={"small"} />
    </Tooltip>
  ),
}

