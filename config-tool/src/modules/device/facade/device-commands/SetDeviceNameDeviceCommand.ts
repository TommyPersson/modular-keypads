import { DeviceCommand } from "./DeviceCommand"

export class SetDeviceNameDeviceCommand extends DeviceCommand<void> {
  override type: string = "set.device.name"

  constructor(
    private readonly deviceId: string,
    private readonly name: string,
  ) {
    super()
  }

  override get arguments(): string[] {
    return [this.deviceId, this.name]
  }

  override parseResponse(_: string[]): void {
  }
}