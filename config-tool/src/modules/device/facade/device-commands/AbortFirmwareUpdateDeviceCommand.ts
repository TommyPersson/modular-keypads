import { DeviceCommand } from "./DeviceCommand"

export class AbortFirmwareUpdateDeviceCommand extends DeviceCommand<void> {
  override type: string = "abort.firmware.update"

  override parseResponse(_: string[]): void {
  }
}
