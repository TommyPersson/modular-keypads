import SaveOutlinedIcon from "@mui/icons-material/SaveOutlined"
import { globalDeviceFacade } from "@src/modules/device/context"
import { SaveMacroDeviceCommand } from "@src/modules/device/facade/device-commands/SaveMacroDeviceCommand"
import { ListKeyBindingsQueryKey } from "@src/modules/key-bindings/queries"
import { type MacroDefinition, MacroDefinitionType } from "@src/modules/macros/models"
import { ListStoredMacrosQueryKey } from "@src/modules/macros/queries"
import type { Command } from "@src/utils/commands"
import { queryClient } from "@src/utils/queryClient"

// Results in the saved macro, with the ID the device assigned to it when it's new
export const SaveMacroCommand: Command<{ macro: MacroDefinition }, MacroDefinition> = {
  mutationOptions: ({
    mutationKey: ["macros", "SaveMacroCommand"],
    mutationFn: async ({ macro }) => {
      const id = await globalDeviceFacade.value.executeCommand(new SaveMacroDeviceCommand(macro))
      return { ...macro, id }
    },
    onSuccess: async () => {
      await queryClient.invalidateQueries({ queryKey: ListStoredMacrosQueryKey })
      await queryClient.invalidateQueries({ queryKey: ListKeyBindingsQueryKey })
    },
  }),
  label: "Save Macro",
  icon: <SaveOutlinedIcon />,
  errorTitle: "Unable to save macro",
  canExecute: ({ macro }) => {
    if (macro.type === MacroDefinitionType.Shortcut) {
      return macro.shortcut.hidCode > 0
    }

    return true
  },
}
