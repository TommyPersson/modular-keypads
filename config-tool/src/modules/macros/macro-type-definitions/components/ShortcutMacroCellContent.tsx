import { Chip, Stack, Tooltip } from "@mui/material"
import type { ShortcutMacroDefinition } from "@src/modules/macros/models"
import { ShortcutVisualization } from "./ShortcutVisualization"

export const ShortcutMacroCellContent = (props: {
  macro: ShortcutMacroDefinition
}) => {
  const { macro } = props
  return (
    <Stack direction={"row"} gap={1} alignItems={"center"}>
      <ShortcutVisualization shortcut={macro.shortcut} />
      {macro.holdModifiers && (
        <Tooltip title={"Modifiers stay held between repeats"}>
          <Chip
            label={macro.holdTimeMs > 0 ? `Hold ${macro.holdTimeMs} ms` : "Hold"}
            size={"small"}
            variant={"outlined"}
          />
        </Tooltip>
      )}
    </Stack>
  )
}
