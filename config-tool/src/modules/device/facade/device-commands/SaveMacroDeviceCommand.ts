import { macroTypeDefinitions } from "@src/modules/macros/macro-type-definitions"
import type { MacroDefinition } from "@src/modules/macros/models"
import { commandLineLength, DeviceCommand, MaxCommandLineLength } from "./DeviceCommand"
import { encodeMacroPath } from "./macroNames"

// Macros are saved in one command line, which their name and data share
export function saveMacroCommandLineLength(macro: MacroDefinition): number {
  return commandLineLength(new SaveMacroDeviceCommand(macro))
}

export function fitsSaveMacroCommandLine(macro: MacroDefinition): boolean {
  return saveMacroCommandLineLength(macro) <= MaxCommandLineLength
}

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

    return [
      this.macro.id.toString(),
      encodeMacroPath(this.macro.directory, this.macro.name),
      ...dataArgs]
  }

  override parseResponse(lines: string[]): number {
    return parseInt(lines[0], 10)
  }
}