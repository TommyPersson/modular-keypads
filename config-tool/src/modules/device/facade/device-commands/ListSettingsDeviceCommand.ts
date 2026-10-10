import { type Setting, SettingType } from "@src/modules/settings/models"
import { DeviceCommand } from "./DeviceCommand"

export class ListSettingsDeviceCommand extends DeviceCommand<Setting[]> {
  override type: string = "list.settings"

  override parseResponse(lines: string[]): Setting[] {
    return lines.map((line): Setting | null => {
      const [key, type, ...values] = line.split(":")

      if (type === "uint") {
        const [value, defaultValue, minValue, maxValue] = values.map(it => parseInt(it, 10))
        return { key, type: SettingType.UInt, value, defaultValue, minValue, maxValue }
      }

      return null
    }).filter(it => it !== null)
  }
}
