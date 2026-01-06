import WbIncandescentOutlinedIcon from "@mui/icons-material/WbIncandescentOutlined"
import { globalDeviceFacade } from "@src/modules/device/context"
import {
  FlashDeviceIdentificationLightsDeviceCommand
} from "@src/modules/device/facade/device-commands/FlashDeviceIdentificationLightsDeviceCommand"
import type { Command } from "@src/utils/commands"

export const FlashDeviceIdentificationLightsCommand: Command<{ deviceId: string }> = {
  mutationOptions: ({
    mutationKey: ["device", "FlashDeviceIdentificationLights"],
    mutationFn: async ({ deviceId }) => {
      await globalDeviceFacade.value.executeCommand(new FlashDeviceIdentificationLightsDeviceCommand(deviceId, 3_000))
    },
    onSuccess: async () => {

    },
  }),
  label: "Flash device identification lights",
  icon: <WbIncandescentOutlinedIcon />,
  errorTitle: "Unable flash device identification lights",
}
