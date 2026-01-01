import AddOutlinedIcon from "@mui/icons-material/AddOutlined"
import { Chip, Stack } from "@mui/material"
import { keyboadKeyCodes } from "@src/modules/key-bindings/data"
import type { Shortcut } from "@src/modules/key-bindings/models"

export const ShortcutVisualization = (props: {
  shortcut: Shortcut
}) => {
  const { shortcut } = props

  const modifiersLabel = shortcut.modifiers
    .map(hidCode => keyboadKeyCodes.byHidCode[hidCode])
    .map(it => it.hidDescription)
    .join(" + ")

  const hidKeyCode = keyboadKeyCodes.byHidCode[shortcut.hidCode]

  const keyCodeLabel = (
    <>
      <strong>{hidKeyCode.hidDescription}</strong>&nbsp;
      (0x{hidKeyCode.hidCode.toString(16).padStart(2, "0")})
    </>
  )

  return (
    <Stack direction={"row"} alignItems={"center"}>
      {shortcut.modifiers.length > 0 && (
        <>
          <Chip label={modifiersLabel} size={"small"} color={"secondary"} />
          <AddOutlinedIcon fontSize={"small"} />
        </>
      )}
      <Chip label={keyCodeLabel} size={"small"} color={"primary"} />
    </Stack>
  )
}