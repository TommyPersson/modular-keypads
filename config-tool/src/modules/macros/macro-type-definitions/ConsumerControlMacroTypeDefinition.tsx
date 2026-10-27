import EmojiPeopleOutlinedIcon from "@mui/icons-material/EmojiPeopleOutlined"
import { Alert, Tooltip } from "@mui/material"
import {
  type ConsumerControlMacroDefinition,
  DeviceFields,
  deviceHex,
  MacroDefinitionType,
  MacroTypeCategory,
  type MacroTypeDefinition,
  serializeDeviceFields
} from "@src/modules/macros/models"
import { ConsumerControlMacroCellContent } from "./components/ConsumerControlMacroCellContent"
import { consumerControlLabel, texts } from "./components/macroLabels"
import { ConsumerControlMacroDefinitionEditor } from "./components/ConsumerControlMacroDefinitionEditor"


export const ConsumerControlMacroTypeDefinition: MacroTypeDefinition<ConsumerControlMacroDefinition> = {
  type: MacroDefinitionType.ConsumerControl,

  numericCode: 0x02,

  category: MacroTypeCategory.Keyboard,

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
      directory: null,
      type: MacroDefinitionType.ConsumerControl,
      usageId: 0
    }
  },

  parseDeviceResponse(
    id: number,
    directory: string | null,
    name: string,
    dataArgs: string[]
  ): ConsumerControlMacroDefinition | null {
    const fields = DeviceFields.parse(dataArgs)
    if (!fields) {
      return null
    }

    return {
      id,
      directory,
      name,
      type: MacroDefinitionType.ConsumerControl,
      usageId: fields.number("u", 0, 16),
    }
  },

  serializeDeviceArguments(macro: ConsumerControlMacroDefinition): string[] {
    return ["0x02", ...serializeDeviceFields({ u: deviceHex(macro.usageId, 4) })]
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

  searchTexts(macro: ConsumerControlMacroDefinition): string[] {
    return texts(...Object.values(consumerControlLabel(macro.usageId) ?? {}))
  },

  typeVisualizationComponent: () => (
    <Tooltip title={"Consumer Control"}>
      <EmojiPeopleOutlinedIcon fontSize={"small"} />
    </Tooltip>
  ),
}

