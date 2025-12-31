import { DeviceCommand } from "./DeviceCommand"

export class ResetDeviceDeviceCommand extends DeviceCommand<void> {
  override type: string = "reset.device"

  override parseResponse(_: string[]): void {
  }
}