import NetworkPingOutlinedIcon from "@mui/icons-material/NetworkPingOutlined"
import { globalDeviceFacade } from "@src/modules/device/context"
import { PingDeviceCommand as PingDeviceDeviceCommand } from "@src/modules/device/facade/device-commands/PingDeviceCommand"
import type { Command } from "@src/utils/commands"

// The response is shown in the device serial traffic
export const PingDeviceCommand: Command<object> = {
  mutationOptions: ({
    mutationKey: ["device-debugger", "PingDevice"],
    mutationFn: async () => {
      await globalDeviceFacade.value.executeCommand(new PingDeviceDeviceCommand())
    },
  }),
  label: "Ping",
  icon: <NetworkPingOutlinedIcon />,
  errorTitle: "Unable to ping device",
}
