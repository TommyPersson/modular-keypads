import { DeviceCommand } from "./DeviceCommand"

export class PingDeviceCommand extends DeviceCommand<string> {
  type: string = "ping"

  override parseResponse(lines: string[]): string {
    return lines[0] ?? ""
  }
}