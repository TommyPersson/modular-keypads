import AddOutlinedIcon from "@mui/icons-material/AddOutlined"
import { Chip, Stack } from "@mui/material"
import type { ReactNode } from "react"
import { modifiersLabel } from "./macroLabels"

// Shows the modifiers, if any, followed by a plus sign and what they modify
export const ModifiersVisualization = (props: {
  modifiers: number[]
  children: ReactNode
}) => {
  const { modifiers, children } = props

  const label = modifiersLabel(modifiers)

  return (
    <Stack direction={"row"} alignItems={"center"}>
      {label && (
        <>
          <Chip label={label} size={"small"} color={"secondary"} />
          <AddOutlinedIcon fontSize={"small"} />
        </>
      )}
      {children}
    </Stack>
  )
}
