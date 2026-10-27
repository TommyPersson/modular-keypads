import PlaylistPlayOutlinedIcon from "@mui/icons-material/PlaylistPlayOutlined"
import { Tooltip } from "@mui/material"
import {
  DeviceFields,
  MacroDefinitionType,
  MacroTypeCategory,
  type MacroTypeDefinition,
  type SequenceMacroDefinition,
  type SequenceStep,
  serializeDeviceFields
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

  // The steps are one field, separated by semicolons, with the fields of each step separated by slashes. A step field
  // is a letter followed by a number: the macro to run (m) and the delay (d), or for loops, the index of the step to go
  // back to (l) and how many times to go back (r) instead of the macro: "s=m12/d0;m15/d250;l0/r3/d500". Commas
  // separate the arguments of commands, and colons the parts of the lines listing the macros.
  //
  // Unknown step fields are skipped, but steps that are neither macros nor loops make the whole sequence invalid, as
  // skipping them would change what it does and which steps its loops go back to.
  parseDeviceResponse(id: number, directory: string | null, name: string, dataArgs: string[]): SequenceMacroDefinition | null {
    const fields = DeviceFields.parse(dataArgs)
    const stepsField = fields?.get("s")
    if (!stepsField) {
      return null
    }

    const steps: SequenceStep[] = []
    for (const stepValue of stepsField.split(";")) {
      const stepFields = new Map<string, number>()
      for (const field of stepValue.split("/")) {
        if (field.length < 2) {
          return null
        }
        if (!stepFields.has(field[0])) {
          stepFields.set(field[0], parseInt(field.substring(1), 10) || 0)
        }
      }

      const macroId = stepFields.get("m")
      const targetStep = stepFields.get("l")
      const delayMs = stepFields.get("d") ?? 0
      if (macroId !== undefined && targetStep === undefined) {
        steps.push({ macroId, delayMs })
      } else if (targetStep !== undefined && macroId === undefined) {
        steps.push({ macroId: 0, delayMs, loop: { targetStep, repeats: stepFields.get("r") ?? 0 } })
      } else {
        return null
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
    const steps = macro.steps.map(step =>
      step.loop
        ? `l${step.loop.targetStep}/r${step.loop.repeats}/d${step.delayMs}`
        : `m${step.macroId}/d${step.delayMs}`
    )
    return ["0x08", ...serializeDeviceFields({ s: steps.join(";") })]
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
