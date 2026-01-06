import EditOutlinedIcon from "@mui/icons-material/EditOutlined"
import { globalDeviceFacade } from "@src/modules/device/context"
import { SetDeviceNameDeviceCommand } from "@src/modules/device/facade/device-commands/SetDeviceNameDeviceCommand"
import type { Command } from "@src/utils/commands"

export const RenameDeviceCommand: Command<{ deviceId: string, deviceName: string }> = {
  mutationOptions: ({
    mutationKey: ["device", "RenameDevice"],
    mutationFn: async ({ deviceId, deviceName }) => {
      await globalDeviceFacade.value.executeCommand(new SetDeviceNameDeviceCommand(deviceId, deviceName))
    },
    onSuccess: async () => {
      // TODO invalidate/reset device
    },
  }),
  label: "Rename device",
  icon: <EditOutlinedIcon />,
  errorTitle: "Unable rename device",
}
