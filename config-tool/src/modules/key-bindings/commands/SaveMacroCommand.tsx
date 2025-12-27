import SaveOutlinedIcon from "@mui/icons-material/SaveOutlined"
import { globalDeviceFacade } from "@src/modules/device/context"
import { type MacroDefinition, MacroDefinitionType } from "@src/modules/key-bindings/models"
import { ListStoredMacrosQueryKey } from "@src/modules/key-bindings/queries"
import type { Command } from "@src/utils/commands"
import { queryClient } from "@src/utils/queryClient"

export const SaveMacroCommand: Command<{ macro: MacroDefinition }> = {
  mutationOptions: ({
    mutationKey: ["key-bindings", "SaveMacroCommand"],
    mutationFn: async ({ macro }) => {
      await globalDeviceFacade.value.saveMacro(macro)
    },
    onSuccess: async () => {
      await queryClient.invalidateQueries({ queryKey: ListStoredMacrosQueryKey })
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
