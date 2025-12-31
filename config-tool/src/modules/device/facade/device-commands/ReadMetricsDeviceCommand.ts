import type { DeviceMetric } from "../../models"
import { DeviceCommand } from "./DeviceCommand"

export class ReadMetricsDeviceCommand extends DeviceCommand<DeviceMetric[]> {
  override type: string = "read.metrics"

  override parseResponse(lines: string[]): DeviceMetric[] {
    return lines.map(line => {
      const [name, valueStr] = line.split(":")
      return { name, value: parseInt(valueStr) }
    })
  }
}