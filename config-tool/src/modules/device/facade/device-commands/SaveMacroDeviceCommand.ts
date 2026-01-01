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
        const modifiersArg = `0x${createModifierFlags(this.macro.shortcut.modifiers).toString(16).padStart(2, "0")}`
        const hidCodeArg = `0x${this.macro.shortcut.hidCode.toString(16).padStart(2, "0")}`
        return ["0x01", modifiersArg, hidCodeArg]
      }

      if (this.macro.type === MacroDefinitionType.ConsumerControl) {
        const usageIdArg = `0x${this.macro.usageId.toString(16).padStart(4, "0")}`
        return ["0x02", usageIdArg]
      }

      if (this.macro.type === MacroDefinitionType.SystemControl) {
        const codeArg = `0x${this.macro.code.toString(16).padStart(2, "0")}`
        return ["0x03", codeArg]
      }

      throw new Error("Unsupported macro type")
    })()

    return [this.macro.id.toString(), this.macro.name, ...dataArgs]
  }

  override parseResponse(_: string[]): void {
  }
}