import { DeviceCommand } from "./DeviceCommand"

export class FlashDeviceIdentificationLightsDeviceCommand extends DeviceCommand<void> {
  override type: string = "flash.identification.lights"

  constructor(private readonly deviceId: string, private readonly durationMs: number) {
    super()
  }

  override get arguments(): string[] {
    return [this.deviceId, this.durationMs.toString()]
  }

  override parseResponse(_: string[]): void {
  }
}