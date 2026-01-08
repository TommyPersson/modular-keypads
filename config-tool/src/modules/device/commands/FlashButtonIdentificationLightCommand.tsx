import WbIncandescentOutlinedIcon from "@mui/icons-material/WbIncandescentOutlined"
import { globalDeviceFacade } from "@src/modules/device/context"
import {
  FlashButtonIdentificationLightDeviceCommand
} from "@src/modules/device/facade/device-commands/FlashButtonDeviceIdentificationLightDeviceCommand"
import type { Command } from "@src/utils/commands"

export const FlashButtonIdentificationLightCommand: Command<{ deviceId: string, buttonNumber: number }> = {
  mutationOptions: ({
    mutationKey: ["device", "FlashButtonIdentificationLight"],
    mutationFn: async ({ deviceId, buttonNumber }) => {
      await globalDeviceFacade.value.executeCommand(new FlashButtonIdentificationLightDeviceCommand(deviceId, buttonNumber, 3_000))
    },
    onSuccess: async () => {

    },
  }),
  label: "Flash button identification light",
  icon: <WbIncandescentOutlinedIcon />,
  errorTitle: "Unable flash button identification light",
}
