import { macroTypeDefinitions } from "@src/modules/macros/macro-type-definitions"
import type { MacroDefinition } from "@src/modules/macros/models"
import { DeviceCommand } from "./DeviceCommand"

export class ListStoredMacrosDeviceCommand extends DeviceCommand<MacroDefinition[]> {
  override type: string = "list.stored.macros"

  override parseResponse(lines: string[]): MacroDefinition[] {
    const parseMacro = ((line: string): MacroDefinition | null => {
      const [encodedName, restLine] = line.split("=")
      const [idStr, typeStr, ...restParts] = restLine.split(":")
      const id = parseInt(idStr, 16)
      const type = parseInt(typeStr, 16)

      const fullName = decodeURIComponent(encodedName)
      const fullNameParts = fullName.split("/")
      const directoryParts = fullNameParts.slice(0, fullNameParts.length-1)
      const directory: string | null = directoryParts.length > 0 ? directoryParts.join("/") : null
      const name = decodeURIComponent(fullNameParts[fullNameParts.length-1])

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