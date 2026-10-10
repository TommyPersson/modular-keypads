import { DeviceCommand } from "./DeviceCommand"

export class SetSettingDeviceCommand extends DeviceCommand<void> {
  override type: string = "set.setting"

  constructor(
    private readonly key: string,
    private readonly value: string,
  ) {
    super()
  }

  override get arguments(): string[] {
    return [this.key, this.value]
  }

  override parseResponse(_: string[]): void {
  }
}
