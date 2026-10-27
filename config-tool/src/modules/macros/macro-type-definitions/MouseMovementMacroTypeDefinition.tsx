import OpenWithOutlinedIcon from "@mui/icons-material/OpenWithOutlined"
import { Alert, Tooltip } from "@mui/material"
import {
  createModifierFlags,
  DeviceFields,
  deviceHex,
  MacroDefinitionType,
  MacroTypeCategory,
  type MacroTypeDefinition,
  type MouseMovementMacroDefinition,
  parseModifierFlags,
  serializeDeviceFields
} from "@src/modules/macros/models"
import { MouseMovementMacroCellContent } from "./components/MouseMovementMacroCellContent"
import { modifiersLabel, mouseMovementLabel, texts } from "./components/macroLabels"
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

  parseDeviceResponse(id: number, directory: string | null, name: string, dataArgs: string[]): MouseMovementMacroDefinition | null {
    const fields = DeviceFields.parse(dataArgs)
    if (!fields) {
      return null
    }

    return {
      id,
      directory,
      name,
      type: MacroDefinitionType.MouseMovement,
      modifiers: parseModifierFlags(fields.number("m", 0, 16)),
      x: fields.number("x", 0),
      y: fields.number("y", 0),
    }
  },

  serializeDeviceArguments(macro: MouseMovementMacroDefinition): string[] {
    return ["0x07", ...serializeDeviceFields({
      m: deviceHex(createModifierFlags(macro.modifiers), 2),
      x: macro.x,
      y: macro.y,
    })]
  },

  editorInformationComponent: () => (
    <Alert severity={"info"}>
      Movement is sent to the computer in steps of at most 127 pixels. With pointer acceleration, such as the
      <em> Enhance pointer precision</em> setting in Windows, the pointer can then end up moving a different distance
      than set here. Turn the acceleration off for exact movement.
    </Alert>
  ),

  editorComponent: MouseMovementMacroDefinitionEditor,

  macroTableCellComponent: MouseMovementMacroCellContent,

  searchTexts(macro: MouseMovementMacroDefinition): string[] {
    return texts(modifiersLabel(macro.modifiers), mouseMovementLabel(macro.x, macro.y))
  },

  typeVisualizationComponent: () => (
    <Tooltip title={"Mouse Movement"}>
      <OpenWithOutlinedIcon fontSize={"small"} />
    </Tooltip>
  ),
}
