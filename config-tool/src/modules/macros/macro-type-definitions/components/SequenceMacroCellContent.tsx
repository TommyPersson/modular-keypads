import ArrowRightAltOutlinedIcon from "@mui/icons-material/ArrowRightAltOutlined"
import { Chip, Stack } from "@mui/material"
import { useStoredMacros } from "@src/modules/macros/hooks"
import type { SequenceMacroDefinition } from "@src/modules/macros/models"
import { Fragment } from "react"
import { sequenceDelayLabel, sequenceStepLabel } from "./macroLabels"

/**
 * Shows the steps in order, as the names of their macros preceded by their delays, wrapping onto more lines when they
 * don't fit.
 */
export const SequenceMacroCellContent = (props: {
  macro: SequenceMacroDefinition
}) => {
  const { macro } = props

  const { allMacros } = useStoredMacros()

  return (
    <Stack direction={"row"} gap={0.5} alignItems={"center"} flexWrap={"wrap"}>
      {macro.steps.map((step, index) => {
        const delayLabel = sequenceDelayLabel(step.delayMs)
        const isMissing = !allMacros.some(it => it.id === step.macroId)

        return (
          <Fragment key={index}>
            {index > 0 && <ArrowRightAltOutlinedIcon fontSize={"small"} color={"action"} />}
            {delayLabel && <Chip label={delayLabel} size={"small"} variant={"outlined"} />}
            <Chip
              label={<strong>{sequenceStepLabel(step, allMacros)}</strong>}
              size={"small"}
              color={isMissing ? "error" : "primary"}
            />
          </Fragment>
        )
      })}
    </Stack>
  )
}
