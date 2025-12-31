import type { DeviceRegisterValue } from "@src/modules/device/models"
import { DeviceCommand } from "./DeviceCommand"

export class ListRegisterValuesDeviceCommand extends DeviceCommand<DeviceRegisterValue[]> {
  override type: string = "list.register.values"

  override parseResponse(lines: string[]): DeviceRegisterValue[] {
    return lines.map(line => {
      const parts = line.split(":")

      return {
        name: parts[0],
        value: parseInt(parts[1], 16)
      }
    })
  }
}