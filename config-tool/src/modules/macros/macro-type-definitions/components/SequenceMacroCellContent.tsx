import ArrowRightAltOutlinedIcon from "@mui/icons-material/ArrowRightAltOutlined"
import RepeatOutlinedIcon from "@mui/icons-material/RepeatOutlined"
import { Box, Chip, Stack } from "@mui/material"
import { useStoredMacros } from "@src/modules/macros/hooks"
import { isValidSequenceStep, type SequenceMacroDefinition } from "@src/modules/macros/models"
import { useIsOverflowing } from "@src/utils/hooks"
import { Fragment, useRef } from "react"
import { sequenceDelayLabel, sequenceStepLabel } from "./macroLabels"

/**
 * Shows the steps in order, as the names of their macros, separated by arrows with the delays before the steps below
 * them, on a single line, so that long sequences take no more room than other macros. The steps that don't fit are cut
 * off, fading out towards the end, so that it's clear that there are more.
 */
export const SequenceMacroCellContent = (props: {
  macro: SequenceMacroDefinition
}) => {
  const { macro } = props

  const { allMacros } = useStoredMacros()

  const ref = useRef<HTMLDivElement>(null)
  const isOverflowing = useIsOverflowing(ref)

  // The steps aren't shrunk, so that they're cut off as a whole rather than each being cut off a bit
  return (
    <Stack
      ref={ref}
      direction={"row"}
      gap={0.5}
      alignItems={"center"}
      sx={{
        minWidth: 0,
        overflow: "hidden",
        "& > *": { flexShrink: 0 },
        maskImage: isOverflowing ? FADE_OUT_MASK : undefined,
      }}
    >
      {macro.steps.map((step, index) => {
        const delayLabel = sequenceDelayLabel(step.delayMs)
        const isMissing = !step.loop && !allMacros.some(it => it.id === step.macroId)

        return (
          <Fragment key={index}>
            {/* Also before the first step when it's delayed, as there's no other place for its delay */}
            {(index > 0 || delayLabel) && <StepArrow delayLabel={delayLabel} />}
            {step.loop ? (
              // Outlined, so that loops stand out from the macros they repeat
              <Chip
                icon={<RepeatOutlinedIcon />}
                label={sequenceStepLabel(step, allMacros)}
                size={"small"}
                variant={"outlined"}
                color={isValidSequenceStep(step, index) ? "primary" : "error"}
              />
            ) : (
              <Chip
                label={<strong>{sequenceStepLabel(step, allMacros)}</strong>}
                size={"small"}
                color={isMissing ? "error" : "primary"}
              />
            )}
          </Fragment>
        )
      })}
    </Stack>
  )
}

const FADE_OUT_MASK = "linear-gradient(to right, black calc(100% - 32px), transparent)"

// The delay is as high as it's moved up into the space below the arrow, and the arrow is moved down as far, so that
// the arrow stays centered with the steps
const DELAY_LABEL_HEIGHT_PX = 10
const DELAY_LABEL_OVERLAP_PX = 8

/**
 * An arrow to the next step, with the delay before that step below it, in small print.
 */
const StepArrow = (props: { delayLabel: string | null }) => {
  const { delayLabel } = props

  if (!delayLabel) {
    return <ArrowRightAltOutlinedIcon fontSize={"small"} color={"action"} />
  }

  return (
    <Stack alignItems={"center"} sx={{ pt: `${DELAY_LABEL_HEIGHT_PX - DELAY_LABEL_OVERLAP_PX}px` }}>
      <ArrowRightAltOutlinedIcon fontSize={"small"} color={"action"} sx={{ mb: `-${DELAY_LABEL_OVERLAP_PX}px` }} />
      <Box
        component={"span"}
        sx={{
          fontSize: 9,
          lineHeight: `${DELAY_LABEL_HEIGHT_PX}px`,
          color: "text.secondary",
          whiteSpace: "nowrap",
        }}
      >
        {delayLabel}
      </Box>
    </Stack>
  )
}
