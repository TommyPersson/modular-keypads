import { DeviceCommand } from "./DeviceCommand"

export class SetDeviceAddressDeviceCommand extends DeviceCommand<void> {
  override type: string = "set.device.address"

  constructor(
    private readonly address: number,
  ) {
    super()
  }

  override get arguments(): string[] {
    return [`0x${this.address.toString(16)}`]
  }

  override parseResponse(_: string[]): void {
  }
}