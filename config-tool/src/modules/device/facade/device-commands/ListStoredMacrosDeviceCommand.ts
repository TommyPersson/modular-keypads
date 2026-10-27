import { macroTypeDefinitions } from "@src/modules/macros/macro-type-definitions"
import type { MacroDefinition } from "@src/modules/macros/models"
import { DeviceCommand } from "./DeviceCommand"
import { decodeMacroPath } from "./macroNames"

export class ListStoredMacrosDeviceCommand extends DeviceCommand<MacroDefinition[]> {
  override type: string = "list.stored.macros"

  override parseResponse(lines: string[]): MacroDefinition[] {
    const parseMacro = ((line: string): MacroDefinition | null => {
      // The data fields have equal signs of their own, while the URI encoded path has none
      const separator = line.indexOf("=")
      const encodedName = line.substring(0, separator)
      const restLine = line.substring(separator + 1)
      const [idStr, typeStr, ...restParts] = restLine.split(":")
      const id = parseInt(idStr, 16)
      const type = parseInt(typeStr, 16)

      const { directory, name } = decodeMacroPath(encodedName)

      const dataArgs = restParts

      const typeDefinition = macroTypeDefinitions.byCode[type]
      if (typeDefinition) {
        return typeDefinition.parseDeviceResponse(id, directory, name, dataArgs)
      }

      return null
    })

    return lines.map(parseMacro).filter(it => it) as MacroDefinition[]
  }
}