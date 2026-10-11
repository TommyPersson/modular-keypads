import { Chip } from "@mui/material"
import { keyboadKeyCodes } from "@src/modules/macros/data"
import type { Shortcut } from "@src/modules/macros/models"
import { ModifiersVisualization } from "./ModifiersVisualization"

export const ShortcutVisualization = (props: {
  shortcut: Shortcut
}) => {
  const { shortcut } = props

  const hidKeyCode = keyboadKeyCodes.byHidCode[shortcut.hidCode]

  const keyCodeLabel = (
    <>
      <strong>{hidKeyCode.hidDescription}</strong>&nbsp;
      (0x{hidKeyCode.hidCode.toString(16).padStart(2, "0")})
    </>
  )

  return (
    <ModifiersVisualization modifiers={shortcut.modifiers}>
      <Chip label={keyCodeLabel} size={"small"} color={"primary"} />
    </ModifiersVisualization>
  )
}
