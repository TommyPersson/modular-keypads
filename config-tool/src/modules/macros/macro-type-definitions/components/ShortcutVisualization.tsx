import { Chip } from "@mui/material"
import type { Shortcut } from "@src/modules/macros/models"
import { keyCodeLabel } from "./macroLabels"
import { ModifiersVisualization } from "./ModifiersVisualization"

export const ShortcutVisualization = (props: {
  shortcut: Shortcut
}) => {
  const { shortcut } = props

  const { description, code } = keyCodeLabel(shortcut.hidCode)

  return (
    <ModifiersVisualization modifiers={shortcut.modifiers}>
      <Chip label={<><strong>{description}</strong>&nbsp;({code})</>} size={"small"} color={"primary"} />
    </ModifiersVisualization>
  )
}
