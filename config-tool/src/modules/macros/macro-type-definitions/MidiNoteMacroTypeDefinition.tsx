import PianoOutlinedIcon from "@mui/icons-material/PianoOutlined"
import { Tooltip } from "@mui/material"
import {
  DeviceFields,
  MacroDefinitionType,
  MacroTypeCategory,
  type MacroTypeDefinition,
  type MidiNoteMacroDefinition,
  serializeDeviceFields
} from "@src/modules/macros/models"
import { MidiNoteMacroCellContent } from "./components/MidiNoteMacroCellContent"
import { midiChannelLabel, midiNoteLabel, texts } from "./components/macroLabels"
import { MidiNoteMacroDefinitionEditor } from "./components/MidiNoteMacroDefinitionEditor"


export const MidiNoteMacroTypeDefinition: MacroTypeDefinition<MidiNoteMacroDefinition> = {
  type: MacroDefinitionType.MidiNote,

  numericCode: 0x09,

  category: MacroTypeCategory.Midi,

  displayName: "MIDI Note",

  description: (
    <>
      Sends a <strong>MIDI</strong> note, which apps controlled with MIDI, such as OBS through a MIDI plugin, can map
      to an action. The app identifies it by note number and channel. Note off is sent once the duration has passed,
      or when the key is released.
    </>
  ),

  handles(type: MacroDefinitionType): boolean {
    return type === this.type
  },

  createDefaultMacroDefinition(name: string): MidiNoteMacroDefinition {
    return {
      id: 0,
      directory: null,
      name,
      type: MacroDefinitionType.MidiNote,
      channel: 1,
      note: 60,
      velocity: 127,
      durationMs: 100,
      endOnRelease: false,
    }
  },

  parseDeviceResponse(id: number, directory: string | null, name: string, dataArgs: string[]): MidiNoteMacroDefinition | null {
    const fields = DeviceFields.parse(dataArgs)
    if (!fields) {
      return null
    }

    return {
      id,
      directory,
      name,
      type: MacroDefinitionType.MidiNote,
      channel: fields.number("c", 1),
      note: fields.number("n", 60),
      velocity: fields.number("v", 127),
      durationMs: fields.number("d", 0),
      endOnRelease: fields.flag("r"),
    }
  },

  serializeDeviceArguments(macro: MidiNoteMacroDefinition): string[] {
    return ["0x09", ...serializeDeviceFields({
      c: macro.channel,
      n: macro.note,
      v: macro.velocity,
      d: macro.durationMs,
      r: macro.endOnRelease,
    })]
  },

  editorInformationComponent: () => null,

  editorComponent: MidiNoteMacroDefinitionEditor,

  macroTableCellComponent: MidiNoteMacroCellContent,

  searchTexts(macro: MidiNoteMacroDefinition): string[] {
    return texts(midiNoteLabel(macro.note), midiChannelLabel(macro.channel))
  },

  typeVisualizationComponent: () => (
    <Tooltip title={"MIDI Note"}>
      <PianoOutlinedIcon fontSize={"small"} />
    </Tooltip>
  ),
}
