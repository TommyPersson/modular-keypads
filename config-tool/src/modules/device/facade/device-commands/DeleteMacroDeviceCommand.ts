import { DeviceCommand } from "./DeviceCommand"

export class DeleteMacroDeviceCommand extends DeviceCommand<void> {
  override type: string = "delete.macro"

  constructor(private readonly macroId: number) {
    super()
  }

  override get arguments(): string[] {
    return [this.macroId.toString()]
  }

  override parseResponse(_: string[]): void {
  }
}