import PlaylistPlayOutlinedIcon from "@mui/icons-material/PlaylistPlayOutlined"
import { Tooltip } from "@mui/material"
import {
  MacroDefinitionType,
  MacroTypeCategory,
  type MacroTypeDefinition,
  type SequenceMacroDefinition,
  type SequenceStep
} from "@src/modules/macros/models"
import { sequenceDelayLabel, sequenceStepLabel, texts } from "./components/macroLabels"
import { SequenceMacroCellContent } from "./components/SequenceMacroCellContent"
import { SequenceMacroDefinitionEditor } from "./components/SequenceMacroDefinitionEditor"


export const SequenceMacroTypeDefinition: MacroTypeDefinition<SequenceMacroDefinition> = {
  type: MacroDefinitionType.Sequence,

  numericCode: 0x08,

  category: MacroTypeCategory.Combined,

  displayName: "Sequence",

  description: (
    <>
      Runs other macros in order. Each step waits until the text typed by the steps before it is done, and then for
      its own delay. Sequences can't be steps of other sequences.
    </>
  ),

  handles(type: MacroDefinitionType): boolean {
    return type === this.type
  },

  createDefaultMacroDefinition(name: string): SequenceMacroDefinition {
    return {
      id: 0,
      directory: null,
      name,
      type: MacroDefinitionType.Sequence,
      steps: [{ macroId: 0, delayMs: 0 }],
    }
  },

  // The steps are one argument, with the macro ID and delay of each step separated by semicolons: "12;0;15;250".
  // Commas separate the arguments of commands, and colons the parts of the lines listing the macros.
  parseDeviceResponse(id: number, directory: string | null, name: string, dataArgs: string[]): SequenceMacroDefinition {
    const numbers = (dataArgs[0] ?? "").split(";").map(it => parseInt(it, 10))

    const steps: SequenceStep[] = []
    for (let i = 0; i + 1 < numbers.length; i += 2) {
      steps.push({ macroId: numbers[i], delayMs: numbers[i + 1] })
    }

    return {
      id,
      directory,
      name,
      type: MacroDefinitionType.Sequence,
      steps,
    }
  },

  serializeDeviceArguments(macro: SequenceMacroDefinition): string[] {
    return ["0x08", macro.steps.flatMap(step => [step.macroId, step.delayMs]).join(";")]
  },

  editorInformationComponent: () => null,

  editorComponent: SequenceMacroDefinitionEditor,

  macroTableCellComponent: SequenceMacroCellContent,

  searchTexts(macro: SequenceMacroDefinition, macros): string[] {
    return texts(...macro.steps.flatMap(step => [sequenceDelayLabel(step.delayMs), sequenceStepLabel(step, macros)]))
  },

  typeVisualizationComponent: () => (
    <Tooltip title={"Sequence"}>
      <PlaylistPlayOutlinedIcon fontSize={"small"} />
    </Tooltip>
  ),
}
