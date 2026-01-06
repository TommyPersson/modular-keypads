import WbIncandescentOutlinedIcon from "@mui/icons-material/WbIncandescentOutlined"
import { globalDeviceFacade } from "@src/modules/device/context"
import {
  EnableIdentificationLightsDeviceCommand
} from "@src/modules/device/facade/device-commands/EnableIdentificationLightsDeviceCommand"
import type { Command } from "@src/utils/commands"

export const EnableIdentificationLightsCommand: Command<{ deviceId: string }> = {
  mutationOptions: ({
    mutationKey: ["device", "EnableIdentificationLights"],
    mutationFn: async ({ deviceId }) => {
      await globalDeviceFacade.value.executeCommand(new EnableIdentificationLightsDeviceCommand(deviceId))
    },
    onSuccess: async () => {

    },
  }),
  label: "Enable identification lights",
  icon: <WbIncandescentOutlinedIcon />,
  errorTitle: "Unable enable identification lights",
}
