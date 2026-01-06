import { DeviceCommand } from "./DeviceCommand"

export class EnableIdentificationLightsDeviceCommand extends DeviceCommand<void> {
  override type: string = "enable.identification.lights"

  constructor(private readonly deviceId: string) {
    super()
  }

  override get arguments(): string[] {
    return [this.deviceId]
  }

  override parseResponse(_: string[]): void {
  }
}