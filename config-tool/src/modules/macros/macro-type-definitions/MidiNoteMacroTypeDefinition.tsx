import PianoOutlinedIcon from "@mui/icons-material/PianoOutlined"
import { Tooltip } from "@mui/material"
import {
  MacroDefinitionType,
  MacroTypeCategory,
  type MacroTypeDefinition,
  type MidiNoteMacroDefinition
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

  parseDeviceResponse(id: number, directory: string | null, name: string, dataArgs: string[]): MidiNoteMacroDefinition {
    const [channelArg, noteArg, velocityArg, durationMsArg, endOnReleaseArg] = dataArgs

    return {
      id,
      directory,
      name,
      type: MacroDefinitionType.MidiNote,
      channel: parseInt(channelArg, 10),
      note: parseInt(noteArg, 10),
      velocity: parseInt(velocityArg, 10),
      durationMs: parseInt(durationMsArg, 10),
      endOnRelease: endOnReleaseArg === "1",
    }
  },

  serializeDeviceArguments(macro: MidiNoteMacroDefinition): string[] {
    const endOnReleaseArg = macro.endOnRelease ? "1" : "0"
    return ["0x09", `${macro.channel}`, `${macro.note}`, `${macro.velocity}`, `${macro.durationMs}`, endOnReleaseArg]
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
