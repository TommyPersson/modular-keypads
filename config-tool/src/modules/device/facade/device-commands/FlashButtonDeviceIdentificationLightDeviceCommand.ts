import { DeviceCommand } from "./DeviceCommand"

export class FlashButtonIdentificationLightDeviceCommand extends DeviceCommand<void> {
  override type: string = "flash.button.identification.light"

  constructor(
    private readonly deviceId: string,
    private readonly buttonNumber: number,
    private readonly durationMs: number
  ) {
    super()
  }

  override get arguments(): string[] {
    return [this.deviceId, this.buttonNumber.toString(), this.durationMs.toString()]
  }

  override parseResponse(_: string[]): void {
  }
}