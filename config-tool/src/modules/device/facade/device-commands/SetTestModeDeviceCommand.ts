import { DeviceCommand } from "./DeviceCommand"

export class SetTestModeDeviceCommand extends DeviceCommand<void> {
  override type: string = "set.test.mode"

  constructor(private readonly enabled: boolean) {
    super()
  }

  get arguments(): string[] {
    return [this.enabled.toString()]
  }

  override parseResponse(_: string[]): void {
  }
}