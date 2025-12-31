import { DeviceCommand } from "./DeviceCommand"

export class GetTestModeDeviceCommand extends DeviceCommand<boolean> {
  override type: string = "get.test.mode"

  override parseResponse(lines: string[]): boolean {
    return lines[0] === "true"
  }
}