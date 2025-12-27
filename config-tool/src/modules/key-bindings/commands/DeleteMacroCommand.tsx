import DeleteForeverOutlinedIcon from "@mui/icons-material/DeleteForeverOutlined"
import { globalDeviceFacade } from "@src/modules/device/context"
import type { MacroDefinition } from "@src/modules/key-bindings/models"
import { ListStoredMacrosQueryKey } from "@src/modules/key-bindings/queries"
import type { Command } from "@src/utils/commands"
import { queryClient } from "@src/utils/queryClient"

export const DeleteMacroCommand: Command<{ macro: MacroDefinition }> = {
  mutationOptions: ({
    mutationKey: ["key-bindings", "DeleteMacroCommand"],
    mutationFn: async ({ macro }) => {
      await globalDeviceFacade.value.deleteMacro(macro.id)
    },
    onSuccess: async () => {
      await queryClient.invalidateQueries({ queryKey: ListStoredMacrosQueryKey })
    }
  }),
  label: "Delete Macro",
  icon: <DeleteForeverOutlinedIcon />,
  color: "error",
  errorTitle: "Unable to delete macro",
  confirmText: ({ macro }) =>
    <>Are you sure you want to <strong>delete</strong> the macro <em>{macro.name}</em>?</>,
  confirmColor: "error",
}
