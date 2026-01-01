import { type MacroDefinition } from "@src/modules/key-bindings/models"
import { macroTypeDefinitionsByCode } from "@src/modules/macro-types"
import { DeviceCommand } from "./DeviceCommand"

export class ListStoredMacrosDeviceCommand extends DeviceCommand<MacroDefinition[]> {
  override type: string = "list.stored.macros"

  override parseResponse(lines: string[]): MacroDefinition[] {
    const parseMacro = ((line: string): MacroDefinition | null => {
      const [name, restLine] = line.split("=")
      const [idStr, typeStr, ...restParts] = restLine.split(":")
      const id = parseInt(idStr, 16)
      const type = parseInt(typeStr, 16)

      const dataArgs = restParts

      const typeDefinition = macroTypeDefinitionsByCode[type]
      if (typeDefinition) {
        return typeDefinition.parseDeviceResponse(id, name,dataArgs)
      }

      return null
    })

    return lines.map(parseMacro).filter(it => it) as MacroDefinition[]
  }
}