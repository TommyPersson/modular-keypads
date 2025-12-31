import {
  type KeyBindingTrigger,
  KeyBindingTriggerType,
  RotaryEncoderDirection
} from "@src/modules/key-bindings/models"
import { DeviceCommand } from "./DeviceCommand"

export class ClearKeyBindingDeviceCommand extends DeviceCommand<void> {
  override type: string = "clear.key.binding"

  constructor(private readonly trigger: KeyBindingTrigger) {
    super()
  }

  override get arguments(): string[] {
    let args: string[]  = []
    if (this.trigger.type === KeyBindingTriggerType.PushButton) {
      args = [
        "0x01",
        this.trigger.deviceId,
        `0x${this.trigger.number.toString(16).padStart(2, "0")}`
      ]
    } else if (this.trigger.type === KeyBindingTriggerType.RotaryEncoder) {
      const directionNumber = this.trigger.direction === RotaryEncoderDirection.Clockwise ? 1 : 2
      args = [
        "0x02",
        this.trigger.deviceId,
        `0x${this.trigger.number.toString(16).padStart(2, "0")}`,
        `0x${directionNumber.toString(16).padStart(2, "0")}`
      ]
    }

    if (args.length === 0) {
      throw new Error("Unknown key binding trigger type")
    }

    return args
  }

  override parseResponse(_: string[]): void {
  }
}