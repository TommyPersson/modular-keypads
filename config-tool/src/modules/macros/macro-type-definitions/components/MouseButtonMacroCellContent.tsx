import { Chip } from "@mui/material"
import { mouseButtons } from "@src/modules/macros/data/mouseButtons"
import type { MouseButtonMacroDefinition } from "@src/modules/macros/models"

export const MouseButtonMacroCellContent = (props: {
  macro: MouseButtonMacroDefinition
}) => {
  const { macro } = props

  const clickedButtons = mouseButtons
    .filter(({ button }) => (macro.buttons & button) !== 0)
    .map(({ name }) => name)
  if (clickedButtons.length === 0) {
    return null
  }

  return (
    <Chip
      label={<strong>{`Click ${clickedButtons.join(" + ")}`}</strong>}
      size={"small"}
      color={"primary"}
    />
  )
}
