import { DeviceCommand } from "./DeviceCommand"

export class BeginFirmwareUpdateDeviceCommand extends DeviceCommand<void> {
  override type: string = "begin.firmware.update"

  constructor(
    private readonly size: number,
  ) {
    super()
  }

  override get arguments(): string[] {
    return [this.size.toString()]
  }

  override parseResponse(_: string[]): void {
  }
}
