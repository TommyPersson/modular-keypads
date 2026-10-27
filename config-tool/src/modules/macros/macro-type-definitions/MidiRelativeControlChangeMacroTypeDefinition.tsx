import TuneOutlinedIcon from "@mui/icons-material/TuneOutlined"
import { Tooltip } from "@mui/material"
import {
  DeviceFields,
  MacroDefinitionType,
  MacroTypeCategory,
  type MacroTypeDefinition,
  type MidiRelativeControlChangeMacroDefinition,
  RelativeControlChangeEncoding,
  serializeDeviceFields
} from "@src/modules/macros/models"
import { MidiRelativeControlChangeMacroCellContent } from "./components/MidiRelativeControlChangeMacroCellContent"
import { midiChannelLabel, midiRelativeControlChangeLabel, texts } from "./components/macroLabels"
import {
  MidiRelativeControlChangeMacroDefinitionEditor
} from "./components/MidiRelativeControlChangeMacroDefinitionEditor"

const parseEncoding = (arg: string): RelativeControlChangeEncoding => {
  const encoding = parseInt(arg, 10)
  return Object.values(RelativeControlChangeEncoding).includes(encoding)
    ? encoding
    : RelativeControlChangeEncoding.BinaryOffset
}

export const MidiRelativeControlChangeMacroTypeDefinition: MacroTypeDefinition<MidiRelativeControlChangeMacroDefinition> = {
  type: MacroDefinitionType.MidiRelativeControlChange,

  numericCode: 0x0a,

  category: MacroTypeCategory.Midi,

  displayName: "MIDI Relative CC",

  description: (
    <>
      Sends a <strong>MIDI</strong> control change that moves a control up or down by an amount, rather than to a
      value. Bind one to each direction of a rotary encoder to turn a slider in an app. The app has to support
      relative control changes, and the encoding has to match what it expects.
    </>
  ),

  handles(type: MacroDefinitionType): boolean {
    return type === this.type
  },

  createDefaultMacroDefinition(name: string): MidiRelativeControlChangeMacroDefinition {
    return {
      id: 0,
      directory: null,
      name,
      type: MacroDefinitionType.MidiRelativeControlChange,
      channel: 1,
      controller: 1,
      amount: 1,
      encoding: RelativeControlChangeEncoding.BinaryOffset,
    }
  },

  parseDeviceResponse(
    id: number,
    directory: string | null,
    name: string,
    dataArgs: string[]
  ): MidiRelativeControlChangeMacroDefinition | null {
    const fields = DeviceFields.parse(dataArgs)
    if (!fields) {
      return null
    }

    return {
      id,
      directory,
      name,
      type: MacroDefinitionType.MidiRelativeControlChange,
      channel: fields.number("c", 1),
      controller: fields.number("n", 0),
      amount: fields.number("a", 0),
      encoding: parseEncoding(fields.get("e") ?? "0"),
    }
  },

  serializeDeviceArguments(macro: MidiRelativeControlChangeMacroDefinition): string[] {
    return ["0x0a", ...serializeDeviceFields({
      c: macro.channel,
      n: macro.controller,
      a: macro.amount,
      e: macro.encoding,
    })]
  },

  editorInformationComponent: () => null,

  editorComponent: MidiRelativeControlChangeMacroDefinitionEditor,

  macroTableCellComponent: MidiRelativeControlChangeMacroCellContent,

  searchTexts(macro: MidiRelativeControlChangeMacroDefinition): string[] {
    return texts(midiRelativeControlChangeLabel(macro.controller, macro.amount), midiChannelLabel(macro.channel))
  },

  typeVisualizationComponent: () => (
    <Tooltip title={"MIDI Relative CC"}>
      <TuneOutlinedIcon fontSize={"small"} />
    </Tooltip>
  ),
}
