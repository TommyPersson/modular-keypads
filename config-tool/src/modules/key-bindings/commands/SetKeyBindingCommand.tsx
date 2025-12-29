import KeyboardHideOutlinedIcon from "@mui/icons-material/KeyboardHideOutlined"
import { globalDeviceFacade } from "@src/modules/device/context"
import { type KeyBindingTrigger } from "@src/modules/key-bindings/models"
import { ListKeyBindingsQueryKeyPrefix } from "@src/modules/key-bindings/queries"
import type { Command } from "@src/utils/commands"
import { queryClient } from "@src/utils/queryClient"

export const SetKeyBindingCommand: Command<{ trigger: KeyBindingTrigger, macroId: number }> = {
  mutationOptions: ({
    mutationKey: ["key-bindings", "SetKeyBinding"],
    mutationFn: async ({ trigger, macroId }) => {
      await globalDeviceFacade.value.setKeyBinding(trigger, macroId)
    },
    onSuccess: async () => {
      await queryClient.invalidateQueries({ queryKey: ListKeyBindingsQueryKeyPrefix })
    },
  }),
  label: "Set key binding",
  icon: <KeyboardHideOutlinedIcon />,
  errorTitle: "Unable to set key binding",
}
