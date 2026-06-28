import { macroTypeDefinitions } from "@src/modules/macros/macro-type-definitions"
import type { MacroDefinition } from "@src/modules/macros/models"
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
      const typeDefinition = macroTypeDefinitions.byType[this.macro.type]
      if (typeDefinition) {
        return typeDefinition.serializeDeviceArguments(this.macro)
      }

      throw new Error("Unsupported macro type")
    })()

    const directoryPart = this.macro.directory ? `${this.macro.directory}/` : ""
    const fullName = directoryPart + encodeURIComponent(this.macro.name)

    return [
      this.macro.id.toString(),
      encodeURIComponent(fullName),
      ...dataArgs]
  }

  override parseResponse(_: string[]): void {
  }
}