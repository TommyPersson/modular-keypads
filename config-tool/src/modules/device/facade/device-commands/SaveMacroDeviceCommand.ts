import { type MacroDefinition } from "@src/modules/key-bindings/models"
import { macroTypeDefinitionsByType } from "@src/modules/macro-types"
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
      const typeDefinition = macroTypeDefinitionsByType[this.macro.type]
      if (typeDefinition) {
        return typeDefinition.serializeDeviceArguments(this.macro)
      }

      throw new Error("Unsupported macro type")
    })()

    return [this.macro.id.toString(), this.macro.name, ...dataArgs]
  }

  override parseResponse(_: string[]): void {
  }
}