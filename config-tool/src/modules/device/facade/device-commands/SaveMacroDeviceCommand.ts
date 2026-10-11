import { macroTypeDefinitions } from "@src/modules/macros/macro-type-definitions"
import type { MacroDefinition } from "@src/modules/macros/models"
import { DeviceCommand } from "./DeviceCommand"

// Responds with the ID of the saved macro, which the device assigns to new macros
export class SaveMacroDeviceCommand extends DeviceCommand<number> {
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

  override parseResponse(lines: string[]): number {
    return parseInt(lines[0], 10)
  }
}