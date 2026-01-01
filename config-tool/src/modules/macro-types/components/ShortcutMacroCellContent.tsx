import type { ShortcutMacroDefinition } from "@src/modules/key-bindings/models"
import { ShortcutVisualization } from "./ShortcutVisualization"

export const ShortcutMacroCellContent = (props: {
  macro: ShortcutMacroDefinition
}) => {
  const { macro } = props
  return (
    <ShortcutVisualization shortcut={macro.shortcut} />
  )
}