import { DeviceCommand } from "./DeviceCommand"

export class SetDeviceTypeDeviceCommand extends DeviceCommand<void> {
  override type: string = "set.device.type"

  constructor(
    private readonly deviceType: string,
  ) {
    super()
  }

  override get arguments(): string[] {
    return [this.deviceType]
  }

  override parseResponse(_: string[]): void {
  }
}