import { createModifierFlags, type MacroDefinition, MacroDefinitionType } from "@src/modules/key-bindings/models"
import { DeviceCommand } from "./DeviceCommand"

export class SaveMacroDeviceCommand extends DeviceCommand<void> {
  override type: string = "save.macro"

  constructor(
    private readonly macro: MacroDefinition,
  ) {
    super()
  }

  override get arguments(): string[] {
    const dataArgs = (() => {
      if (this.macro.type === MacroDefinitionType.Shortcut) {
        const modifiersStr = `0x${createModifierFlags(this.macro.shortcut.modifiers).toString(16).padStart(2, "0")}`
        const hidCodeStr = `0x${this.macro.shortcut.hidCode.toString(16).padStart(2, "0")}`
        return [
          "0x01", // Shortcut
          `${modifiersStr}`,
          `${hidCodeStr}`,
        ]
      }

      if (this.macro.type === MacroDefinitionType.ConsumerControl) {
        return [
          "0x02", // Consumer Control
          `0x${this.macro.usageId.toString(16).padStart(4, "0")}`
        ]
      }

      throw new Error("Unsupported macro type")
    })()

    return [this.macro.id.toString(), this.macro.name, ...dataArgs]
  }

  override parseResponse(_: string[]): void {
  }
}