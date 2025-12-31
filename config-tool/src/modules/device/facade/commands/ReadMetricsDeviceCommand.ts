import type { DeviceMetric } from "../../models"
import type { DeviceCommand } from "./DeviceCommand"

export class ReadMetricsDeviceCommand implements DeviceCommand<DeviceMetric[]> {
  type: string = "read.metrics"

  arguments: string[] = []

  parseResponse(lines: string[]): DeviceMetric[] {
    return lines.map(line => {
      const [name, valueStr] = line.split(":")
      return { name, value: parseInt(valueStr) }
    })
  }
}