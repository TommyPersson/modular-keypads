import TextFieldsOutlinedIcon from "@mui/icons-material/TextFieldsOutlined"
import { Tooltip } from "@mui/material"
import { TextMacroCellContent } from "@src/modules/macros/macro-type-definitions/components/TextMacroCellContent"
import {
  TextMacroDefinitionEditor
} from "@src/modules/macros/macro-type-definitions/components/TextMacroDefinitionEditor"
import { MacroDefinitionType, type MacroTypeDefinition, type TextMacroDefinition } from "@src/modules/macros/models"


export const TextMacroTypeDefinition: MacroTypeDefinition<TextMacroDefinition> = {
  type: MacroDefinitionType.Text,

  numericCode: 0x04,

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
      name,
      type: MacroDefinitionType.Text,
      text: ""
    }
  },

  parseDeviceResponse(id: number, name: string, dataArgs: string[]): TextMacroDefinition {
    const textArg = dataArgs[0]
    const text = decodeURIComponent(textArg)

    return {
      id,
      name,
      type: MacroDefinitionType.Text,
      text,
    }
  },

  serializeDeviceArguments(macro: TextMacroDefinition): string[] {
    const textArg = encodeURIComponent(macro.text)
    return ["0x04", textArg]
  },

  editorInformationComponent: () => null,

  editorComponent: TextMacroDefinitionEditor,

  macroTableCellComponent: TextMacroCellContent,

  typeVisualizationComponent: () => (
    <Tooltip title={"Text"}>
      <TextFieldsOutlinedIcon fontSize={"small"} />
    </Tooltip>
  ),
}

