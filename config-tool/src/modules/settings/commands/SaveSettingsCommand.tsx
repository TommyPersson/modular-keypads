import SaveOutlinedIcon from "@mui/icons-material/SaveOutlined"
import { globalDeviceFacade } from "@src/modules/device/context"
import { SetSettingDeviceCommand } from "@src/modules/device/facade/device-commands/SetSettingDeviceCommand"
import { ListSettingsQueryKey } from "@src/modules/settings/queries"
import type { Command } from "@src/utils/commands"
import { queryClient } from "@src/utils/queryClient"

export type SettingChange = {
  key: string
  value: string
}

export const SaveSettingsCommand: Command<{ changes: SettingChange[] }> = {
  mutationOptions: ({
    mutationKey: ["settings", "SaveSettings"],
    mutationFn: async ({ changes }) => {
      for (const { key, value } of changes) {
        await globalDeviceFacade.value.executeCommand(new SetSettingDeviceCommand(key, value))
      }
    },
    onSettled: async () => {
      // Also refreshed on failure, as some of the changes may have been saved
      await queryClient.invalidateQueries({ queryKey: ListSettingsQueryKey })
    },
  }),
  label: "Save",
  icon: <SaveOutlinedIcon />,
  errorTitle: "Unable to save settings",
  canExecute: ({ changes }) => changes.length > 0,
}
