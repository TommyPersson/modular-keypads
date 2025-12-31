import {
  type ConsumerControlMacroDefinition,
  type MacroDefinition,
  MacroDefinitionType,
  parseModifierFlags,
  type ShortcutMacroDefinition
} from "@src/modules/key-bindings/models"
import { DeviceCommand } from "./DeviceCommand"

export class ListStoredMacrosDeviceCommand extends DeviceCommand<MacroDefinition[]> {
  override type: string = "list.stored.macros"

  override parseResponse(lines: string[]): MacroDefinition[] {
    const parseMacro = ((line: string): MacroDefinition | null => {
      const [name, restLine] = line.split("=")
      const [idStr, typeStr, ...restParts] = restLine.split(":")
      const id = parseInt(idStr, 16)
      const type = parseInt(typeStr, 16)

      if (type === 0x01) {
        const [modifiersStr, hidCodeStr] = restParts
        const modifiers = parseModifierFlags(parseInt(modifiersStr, 16))
        const hidCode = parseInt(hidCodeStr, 16)

        return {
          id,
          name,
          type: MacroDefinitionType.Shortcut,
          shortcut: {
            modifiers,
            hidCode
          }
        } satisfies ShortcutMacroDefinition
      }

      if (type === 0x02) {
        const usageIdPart = restParts[0]
        const usageId = parseInt(usageIdPart, 16)

        return {
          id,
          name,
          type: MacroDefinitionType.ConsumerControl,
          usageId
        } satisfies ConsumerControlMacroDefinition
      }

      return null
    })

    return lines.map(parseMacro).filter(it => it) as MacroDefinition[]
  }
}