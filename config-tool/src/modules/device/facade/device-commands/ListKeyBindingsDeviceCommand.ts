import { type KeyBinding, KeyBindingTriggerType, RotaryEncoderDirection } from "@src/modules/key-bindings/models"
import { DeviceCommand } from "./DeviceCommand"

export class ListKeyBindingsDeviceCommand extends DeviceCommand<KeyBinding[]> {
  override type: string = "list.key.bindings"

  override parseResponse(lines: string[]): KeyBinding[] {
    return lines.map(line => {
      const parts = line.split(":")
      const type = parseInt(parts[0], 16)
      const deviceId = parts[1]

      if (type == 0x01) { // PushButton
        const number = parseInt(parts[2], 16)
        const macroId = parseInt(parts[3], 16)

        return {
          macroId,
          trigger: {
            type: KeyBindingTriggerType.PushButton,
            deviceId,
            number
          }
        }
      } else if (type == 0x02) { // RotaryEncoder
        const number = parseInt(parts[2], 16)
        const direction = parseInt(parts[3], 16)
        const macroId = parseInt(parts[4], 16)

        return {
          macroId,
          trigger: {
            type: KeyBindingTriggerType.RotaryEncoder,
            deviceId,
            number,
            direction: direction === 0x01 ? RotaryEncoderDirection.Clockwise : RotaryEncoderDirection.CounterClockwise
          }
        }
      }
    }).filter(it => it) as KeyBinding[]
  }
}