import { Chip, Stack, Tooltip } from "@mui/material"
import type { ShortcutMacroDefinition } from "@src/modules/macros/models"
import { shortcutHoldLabel } from "./macroLabels"
import { ShortcutVisualization } from "./ShortcutVisualization"

export const ShortcutMacroCellContent = (props: {
  macro: ShortcutMacroDefinition
}) => {
  const { macro } = props

  const holdLabel = shortcutHoldLabel(macro)

  return (
    <Stack direction={"row"} gap={1} alignItems={"center"}>
      <ShortcutVisualization shortcut={macro.shortcut} />
      {holdLabel && (
        <Tooltip title={"Modifiers stay held between repeats"}>
          <Chip label={holdLabel} size={"small"} variant={"outlined"} />
        </Tooltip>
      )}
    </Stack>
  )
}
