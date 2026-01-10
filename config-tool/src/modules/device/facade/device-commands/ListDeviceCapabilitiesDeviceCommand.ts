import type { DeviceCapability } from "@src/modules/device/models"
import { DeviceCommand } from "./DeviceCommand"

export class ListDeviceCapabilitiesDeviceCommand extends DeviceCommand<DeviceCapability[]> {
  override type: string = "list.device.capabilities"


  constructor(private readonly deviceId: string) {
    super()
  }

  override get arguments(): string[] {
    return [this.deviceId]
  }

  override parseResponse(lines: string[]): DeviceCapability[] {
    return lines.map((line, index) => {
      const [type, ...rest] = line.split(",")
      if (type == "PushButton") {
        return {
          index,
          type,
          number: parseInt(rest[0]),
          hasLed: parseInt(rest[1]) >= 0,
        } satisfies DeviceCapability
      } else if (type == "RotaryEncoder") {
        return {
          index,
          type,
          number: parseInt(rest[0])
        } satisfies DeviceCapability
      } else {
        return null
      }
    }).filter(it => it) as DeviceCapability[]
  }
}