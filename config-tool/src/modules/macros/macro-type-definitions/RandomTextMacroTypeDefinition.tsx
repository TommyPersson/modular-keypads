import ShuffleOutlinedIcon from "@mui/icons-material/ShuffleOutlined"
import { Tooltip } from "@mui/material"
import {
  DeviceFields,
  MacroDefinitionType,
  MacroTypeCategory,
  type MacroTypeDefinition,
  RandomTextFormat,
  type RandomTextMacroDefinition,
  serializeDeviceFields
} from "@src/modules/macros/models"
import { RandomTextMacroCellContent } from "./components/RandomTextMacroCellContent"
import { randomTextFormatLabel, randomTextQuotedLabel, texts } from "./components/macroLabels"
import { RandomTextMacroDefinitionEditor } from "./components/RandomTextMacroDefinitionEditor"


export const RandomTextMacroTypeDefinition: MacroTypeDefinition<RandomTextMacroDefinition> = {
  type: MacroDefinitionType.RandomText,

  numericCode: 0x0c,

  category: MacroTypeCategory.Keyboard,

  displayName: "Random Text",

  description: (
    <>
      Types random text in the chosen format, such as a <strong>UUID</strong>. New text is generated every time the
      macro runs.
    </>
  ),

  handles(type: MacroDefinitionType): boolean {
    return type === this.type
  },

  createDefaultMacroDefinition(name: string): RandomTextMacroDefinition {
    return {
      id: 0,
      directory: null,
      name,
      type: MacroDefinitionType.RandomText,
      format: RandomTextFormat.UuidV4,
      quoted: false,
    }
  },

  parseDeviceResponse(id: number, directory: string | null, name: string, dataArgs: string[]): RandomTextMacroDefinition | null {
    const fields = DeviceFields.parse(dataArgs)
    if (!fields) {
      return null
    }

    return {
      id,
      directory,
      name,
      type: MacroDefinitionType.RandomText,
      format: fields.number("f", 0),
      quoted: fields.flag("q"),
    }
  },

  serializeDeviceArguments(macro: RandomTextMacroDefinition): string[] {
    return ["0x0c", ...serializeDeviceFields({ f: macro.format, q: macro.quoted })]
  },

  editorInformationComponent: () => null,

  editorComponent: RandomTextMacroDefinitionEditor,

  macroTableCellComponent: RandomTextMacroCellContent,

  searchTexts(macro: RandomTextMacroDefinition): string[] {
    return texts(randomTextFormatLabel(macro.format), randomTextQuotedLabel(macro.quoted))
  },

  typeVisualizationComponent: () => (
    <Tooltip title={"Random Text"}>
      <ShuffleOutlinedIcon fontSize={"small"} />
    </Tooltip>
  ),
}
