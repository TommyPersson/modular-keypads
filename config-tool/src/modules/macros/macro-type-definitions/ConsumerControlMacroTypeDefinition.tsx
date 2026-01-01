import EmojiPeopleOutlinedIcon from "@mui/icons-material/EmojiPeopleOutlined"
import { Alert, Tooltip } from "@mui/material"
import {
  type ConsumerControlMacroDefinition,
  MacroDefinitionType,
  type MacroTypeDefinition
} from "@src/modules/macros/models"
import { ConsumerControlMacroCellContent } from "./components/ConsumerControlMacroCellContent"
import { ConsumerControlMacroDefinitionEditor } from "./components/ConsumerControlMacroDefinitionEditor"


export const ConsumerControlMacroTypeDefinition: MacroTypeDefinition<ConsumerControlMacroDefinition> = {
  type: MacroDefinitionType.ConsumerControl,

  numericCode: 0x02,

  displayName: "Consumer Control",

  description: (
    <>
      The <strong>Consumer Control</strong> specification lists many special purpose buttons,
      including media controls.
    </>
  ),

  handles(type: MacroDefinitionType): boolean {
    return type === this.type
  },

  createDefaultMacroDefinition(name: string): ConsumerControlMacroDefinition {
    return {
      id: 0,
      name,
      type: MacroDefinitionType.ConsumerControl,
      usageId: 0
    }
  },

  parseDeviceResponse(id: number, name: string, dataArgs: string[]): ConsumerControlMacroDefinition {
    const usageIdPart = dataArgs[0]
    const usageId = parseInt(usageIdPart, 16)

    return {
      id,
      name,
      type: MacroDefinitionType.ConsumerControl,
      usageId
    }
  },

  serializeDeviceArguments(macro: ConsumerControlMacroDefinition): string[] {
    const usageIdArg = `0x${macro.usageId.toString(16).padStart(4, "0")}`
    return ["0x02", usageIdArg]
  },

  editorInformationComponent: () => (
    <Alert severity={"info"}>
      Keep in mind that the keys shown in HID code list are the raw HID descriptions. The values has not been
      updated to match the current locale. For example, the <code>Q</code> key should be selected instead of
      <code>A</code> in AZERTY layouts.
    </Alert>
  ),

  editorComponent: ConsumerControlMacroDefinitionEditor,

  macroTableCellComponent: ConsumerControlMacroCellContent,

  typeVisualizationComponent: () => (
    <Tooltip title={"Consumer Control"}>
      <EmojiPeopleOutlinedIcon fontSize={"small"} />
    </Tooltip>
  ),
}

