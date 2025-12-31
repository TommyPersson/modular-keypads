import KeyboardHideOutlinedIcon from "@mui/icons-material/KeyboardHideOutlined"
import { globalDeviceFacade } from "@src/modules/device/context"
import { ClearKeyBindingDeviceCommand } from "@src/modules/device/facade/device-commands/ClearKeyBindingDeviceCommand"
import { type KeyBindingTrigger } from "@src/modules/key-bindings/models"
import { ListKeyBindingsQueryKey } from "@src/modules/key-bindings/queries"
import type { Command } from "@src/utils/commands"
import { queryClient } from "@src/utils/queryClient"

export const ClearKeyBindingCommand: Command<{ trigger: KeyBindingTrigger }> = {
  mutationOptions: ({
    mutationKey: ["key-bindings", "ClearKeyBinding"],
    mutationFn: async ({ trigger }) => {
      await globalDeviceFacade.value.executeCommand(new ClearKeyBindingDeviceCommand(trigger))
    },
    onSuccess: async () => {
      await queryClient.invalidateQueries({ queryKey: ListKeyBindingsQueryKey })
    },
  }),
  label: "Clear key binding",
  icon: <KeyboardHideOutlinedIcon />,
  errorTitle: "Unable to clear key binding",
}
