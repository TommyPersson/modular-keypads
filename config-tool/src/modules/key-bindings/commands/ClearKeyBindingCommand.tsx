import KeyboardHideOutlinedIcon from "@mui/icons-material/KeyboardHideOutlined"
import { globalDeviceFacade } from "@src/modules/device/context"
import { type KeyBindingTrigger } from "@src/modules/key-bindings/models"
import { ListKeyBindingsQueryKeyPrefix } from "@src/modules/key-bindings/queries"
import type { Command } from "@src/utils/commands"
import { queryClient } from "@src/utils/queryClient"

export const ClearKeyBindingCommand: Command<{ trigger: KeyBindingTrigger }> = {
  mutationOptions: ({
    mutationKey: ["key-bindings", "ClearKeyBinding"],
    mutationFn: async ({ trigger }) => {
      console.log("ClearKeyBinding", trigger)
      await globalDeviceFacade.value.clearKeyBinding(trigger)
    },
    onSuccess: async () => {
      await queryClient.invalidateQueries({ queryKey: ListKeyBindingsQueryKeyPrefix })
    },
  }),
  label: "Clear key binding",
  icon: <KeyboardHideOutlinedIcon />,
  errorTitle: "Unable to clear key binding",
}
