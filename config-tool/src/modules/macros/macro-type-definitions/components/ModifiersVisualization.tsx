import AddOutlinedIcon from "@mui/icons-material/AddOutlined"
import { Chip, Stack } from "@mui/material"
import { keyboadKeyCodes } from "@src/modules/macros/data"
import type { ReactNode } from "react"

// Shows the modifiers, if any, followed by a plus sign and what they modify
export const ModifiersVisualization = (props: {
  modifiers: number[]
  children: ReactNode
}) => {
  const { modifiers, children } = props

  const modifiersLabel = modifiers
    .map(hidCode => keyboadKeyCodes.byHidCode[hidCode])
    .map(it => it.hidDescription)
    .join(" + ")

  return (
    <Stack direction={"row"} alignItems={"center"}>
      {modifiers.length > 0 && (
        <>
          <Chip label={modifiersLabel} size={"small"} color={"secondary"} />
          <AddOutlinedIcon fontSize={"small"} />
        </>
      )}
      {children}
    </Stack>
  )
}
