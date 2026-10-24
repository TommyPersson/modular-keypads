import LinearScaleOutlinedIcon from "@mui/icons-material/LinearScaleOutlined"
import { Tooltip } from "@mui/material"
import {
  MacroDefinitionType,
  MacroTypeCategory,
  type MacroTypeDefinition,
  type MidiAbsoluteControlChangeMacroDefinition
} from "@src/modules/macros/models"
import { MidiAbsoluteControlChangeMacroCellContent } from "./components/MidiAbsoluteControlChangeMacroCellContent"
import { midiAbsoluteControlChangeLabel, midiChannelLabel, texts } from "./components/macroLabels"
import {
  MidiAbsoluteControlChangeMacroDefinitionEditor
} from "./components/MidiAbsoluteControlChangeMacroDefinitionEditor"


export const MidiAbsoluteControlChangeMacroTypeDefinition: MacroTypeDefinition<MidiAbsoluteControlChangeMacroDefinition> = {
  type: MacroDefinitionType.MidiAbsoluteControlChange,

  numericCode: 0x0b,

  category: MacroTypeCategory.Midi,

  displayName: "MIDI Absolute CC",

  description: (
    <>
      Steps the value of a <strong>MIDI</strong> control up or down, and sends the new value, for apps that only take
      absolute values. Bind one to each direction of a rotary encoder to turn a slider in an app. The keypad keeps the
      value, from 0 to 127, and starts at 0 when it's restarted, as it doesn't know the value in the app. Changes made
      in the app make the slider jump back to the keypad's value on the next step.
    </>
  ),

  handles(type: MacroDefinitionType): boolean {
    return type === this.type
  },

  createDefaultMacroDefinition(name: string): MidiAbsoluteControlChangeMacroDefinition {
    return {
      id: 0,
      directory: null,
      name,
      type: MacroDefinitionType.MidiAbsoluteControlChange,
      channel: 1,
      controller: 1,
      amount: 1,
    }
  },

  parseDeviceResponse(
    id: number,
    directory: string | null,
    name: string,
    dataArgs: string[]
  ): MidiAbsoluteControlChangeMacroDefinition {
    const [channelArg, controllerArg, amountArg] = dataArgs

    return {
      id,
      directory,
      name,
      type: MacroDefinitionType.MidiAbsoluteControlChange,
      channel: parseInt(channelArg, 10),
      controller: parseInt(controllerArg, 10),
      amount: parseInt(amountArg, 10),
    }
  },

  serializeDeviceArguments(macro: MidiAbsoluteControlChangeMacroDefinition): string[] {
    return ["0x0b", `${macro.channel}`, `${macro.controller}`, `${macro.amount}`]
  },

  editorInformationComponent: () => null,

  editorComponent: MidiAbsoluteControlChangeMacroDefinitionEditor,

  macroTableCellComponent: MidiAbsoluteControlChangeMacroCellContent,

  searchTexts(macro: MidiAbsoluteControlChangeMacroDefinition): string[] {
    return texts(midiAbsoluteControlChangeLabel(macro.controller, macro.amount), midiChannelLabel(macro.channel))
  },

  typeVisualizationComponent: () => (
    <Tooltip title={"MIDI Absolute CC"}>
      <LinearScaleOutlinedIcon fontSize={"small"} />
    </Tooltip>
  ),
}
