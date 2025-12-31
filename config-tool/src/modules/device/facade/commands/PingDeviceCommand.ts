import type { DeviceCommand } from "./DeviceCommand"

export class PingDeviceCommand implements DeviceCommand<string> {
  type: string = "ping"

  arguments: string[] = []

  parseResponse(lines: string[]): string {
    return lines[0] ?? ""
  }
}