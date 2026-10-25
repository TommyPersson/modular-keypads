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
      its own delay. Loop steps go back to an earlier step a number of times, to repeat the steps in between.
      Pressing the button of a running sequence again stops it. Sequences can't be steps of other sequences.
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

  // The steps are one argument, with the macro ID and delay of each step separated by semicolons: "12;0;15;250". Loop
  // steps are the index of the step to go back to after an L, how many times to go back, and the delay:
  // "12;0;15;250;L0;3;500". Commas separate the arguments of commands, and colons the parts of the lines listing the
  // macros.
  parseDeviceResponse(id: number, directory: string | null, name: string, dataArgs: string[]): SequenceMacroDefinition {
    const values = (dataArgs[0] ?? "").split(";")

    const steps: SequenceStep[] = []
    let i = 0
    while (i < values.length) {
      if (values[i].startsWith("L") && i + 2 < values.length) {
        steps.push({
          macroId: 0,
          delayMs: parseInt(values[i + 2], 10),
          loop: { targetStep: parseInt(values[i].substring(1), 10), repeats: parseInt(values[i + 1], 10) },
        })
        i += 3
      } else if (i + 1 < values.length) {
        steps.push({ macroId: parseInt(values[i], 10), delayMs: parseInt(values[i + 1], 10) })
        i += 2
      } else {
        break
      }
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
    const values = macro.steps.flatMap(step =>
      step.loop
        ? [`L${step.loop.targetStep}`, step.loop.repeats, step.delayMs]
        : [step.macroId, step.delayMs]
    )
    return ["0x08", values.join(";")]
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
