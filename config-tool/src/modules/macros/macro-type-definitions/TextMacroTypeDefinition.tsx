import TextFieldsOutlinedIcon from "@mui/icons-material/TextFieldsOutlined"
import { Tooltip } from "@mui/material"
import { TextMacroCellContent } from "@src/modules/macros/macro-type-definitions/components/TextMacroCellContent"
import {
  TextMacroDefinitionEditor
} from "@src/modules/macros/macro-type-definitions/components/TextMacroDefinitionEditor"
import {
  DeviceFields,
  MacroDefinitionType,
  MacroTypeCategory,
  type MacroTypeDefinition,
  serializeDeviceFields,
  type TextMacroDefinition
} from "@src/modules/macros/models"


export const TextMacroTypeDefinition: MacroTypeDefinition<TextMacroDefinition> = {
  type: MacroDefinitionType.Text,

  numericCode: 0x04,

  category: MacroTypeCategory.Keyboard,

  displayName: "Text",

  description: (
    <>
      Will simulate typing the given text by sending the corresponding key code for each character.
    </>
  ),

  handles(type: MacroDefinitionType): boolean {
    return type === this.type
  },

  createDefaultMacroDefinition(name: string): TextMacroDefinition {
    return {
      id: 0,
      directory: null,
      name,
      type: MacroDefinitionType.Text,
      text: ""
    }
  },

  // The text is URI encoded, so it has no separators of its own
  parseDeviceResponse(id: number, directory: string | null, name: string, dataArgs: string[]): TextMacroDefinition | null {
    const fields = DeviceFields.parse(dataArgs)
    if (!fields) {
      return null
    }

    return {
      id,
      directory,
      name,
      type: MacroDefinitionType.Text,
      text: decodeURIComponent(fields.get("t") ?? ""),
    }
  },

  serializeDeviceArguments(macro: TextMacroDefinition): string[] {
    return ["0x04", ...serializeDeviceFields({ t: encodeURIComponent(macro.text) })]
  },

  editorInformationComponent: () => null,

  editorComponent: TextMacroDefinitionEditor,

  macroTableCellComponent: TextMacroCellContent,

  searchTexts(macro: TextMacroDefinition): string[] {
    return [macro.text]
  },

  typeVisualizationComponent: () => (
    <Tooltip title={"Text"}>
      <TextFieldsOutlinedIcon fontSize={"small"} />
    </Tooltip>
  ),
}

