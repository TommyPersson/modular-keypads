import ComputerOutlinedIcon from "@mui/icons-material/ComputerOutlined"
import { Tooltip } from "@mui/material"
import {
  DeviceFields,
  deviceHex,
  MacroDefinitionType,
  MacroTypeCategory,
  type MacroTypeDefinition,
  serializeDeviceFields,
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

  parseDeviceResponse(id: number, directory: string | null, name: string, dataArgs: string[]): SystemControlMacroDefinition | null {
    const fields = DeviceFields.parse(dataArgs)
    if (!fields) {
      return null
    }

    return {
      id,
      directory,
      name,
      type: MacroDefinitionType.SystemControl,
      code: fields.number("c", 0, 16),
    }
  },

  serializeDeviceArguments(macro: SystemControlMacroDefinition): string[] {
    return ["0x03", ...serializeDeviceFields({ c: deviceHex(macro.code, 2) })]
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

