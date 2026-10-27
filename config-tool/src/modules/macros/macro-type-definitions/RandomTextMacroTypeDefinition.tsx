import ShuffleOutlinedIcon from "@mui/icons-material/ShuffleOutlined"
import { Tooltip } from "@mui/material"
import {
  MacroDefinitionType,
  MacroTypeCategory,
  type MacroTypeDefinition,
  RandomTextFormat,
  type RandomTextMacroDefinition
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

  parseDeviceResponse(id: number, directory: string | null, name: string, dataArgs: string[]): RandomTextMacroDefinition {
    const [formatArg, quotedArg] = dataArgs

    return {
      id,
      directory,
      name,
      type: MacroDefinitionType.RandomText,
      format: parseInt(formatArg, 10),
      quoted: quotedArg === "1",
    }
  },

  serializeDeviceArguments(macro: RandomTextMacroDefinition): string[] {
    return ["0x0c", `${macro.format}`, macro.quoted ? "1" : "0"]
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
