import AddOutlinedIcon from "@mui/icons-material/AddOutlined"
import ArrowDownwardOutlinedIcon from "@mui/icons-material/ArrowDownwardOutlined"
import ArrowUpwardOutlinedIcon from "@mui/icons-material/ArrowUpwardOutlined"
import DeleteOutlineOutlinedIcon from "@mui/icons-material/DeleteOutlineOutlined"
import RepeatOutlinedIcon from "@mui/icons-material/RepeatOutlined"
import { Box, Button, IconButton, InputAdornment, MenuItem, Stack, TextField, Tooltip, Typography } from "@mui/material"
import { useStoredMacros } from "@src/modules/macros/hooks"
import {
  type MacroDefinition,
  MacroDefinitionType,
  MaxSequenceLoopRepeats,
  MaxSequenceStepDelayMs,
  MaxSequenceSteps,
  type SequenceLoop,
  type SequenceMacroDefinition,
  type SequenceStep
} from "@src/modules/macros/models"
import { MacroAutocomplete } from "@src/modules/macros/ui/components/MacroAutocomplete"
import { type ChangeEvent, useCallback, useMemo } from "react"

/**
 * Edits the steps of a sequence, each a macro and the time to wait before running it, or a loop that goes back to an
 * earlier step. The steps can be moved up and down, and loops keep going back to the same steps when they're moved.
 * Other sequences can't be chosen as steps.
 */
export const SequenceMacroDefinitionEditor = (props: {
  value: SequenceMacroDefinition
  onChange: (value: SequenceMacroDefinition) => void
}) => {
  const { value, onChange } = props
  const { steps } = value

  const { allMacros, macrosById } = useStoredMacros()

  const stepMacros = useMemo(
    () => allMacros.filter(it => it.type !== MacroDefinitionType.Sequence),
    [allMacros]
  )

  const setSteps = useCallback((newSteps: SequenceStep[]) => {
    onChange({ ...value, steps: newSteps })
  }, [value, onChange])

  const changeStep = (index: number, change: Partial<SequenceStep>) => {
    setSteps(steps.map((step, i) => i === index ? { ...step, ...change } : step))
  }

  const changeLoop = (index: number, loop: SequenceLoop) => {
    changeStep(index, { loop })
  }

  const moveStep = (index: number, offset: number) => {
    const otherIndex = index + offset
    const newSteps = [...steps]
    const [step] = newSteps.splice(index, 1)
    newSteps.splice(otherIndex, 0, step)
    // The two steps swap places, which a loop going back to either of them follows
    setSteps(retargetLoops(newSteps, target =>
      target === index ? otherIndex : target === otherIndex ? index : target
    ))
  }

  const removeStep = (index: number) => {
    // Loops going back to the removed step go back to the step that takes its place instead
    setSteps(retargetLoops(steps.filter((_, i) => i !== index), target => target > index ? target - 1 : target))
  }

  const addStep = () => {
    setSteps([...steps, { macroId: 0, delayMs: 0 }])
  }

  const addLoop = () => {
    setSteps([...steps, { macroId: 0, delayMs: 0, loop: { targetStep: 0, repeats: 1 } }])
  }

  return (
    <Stack gap={1.5}>
      {steps.map((step, index) => (
        // Keyed by the macro too, so that a moved step's fields are recreated with its values
        <Stack key={`${index}:${step.loop ? "loop" : step.macroId}`} direction={"row"} gap={1} alignItems={"center"}>
          <Typography color={"text.secondary"} sx={{ width: 24, flexShrink: 0, textAlign: "right" }}>
            {index + 1}.
          </Typography>
          <DelayField value={step.delayMs} onChange={delayMs => changeStep(index, { delayMs })} />
          <Box flex={1} minWidth={0}>
            {step.loop ? (
              <LoopFields index={index} value={step.loop} onChange={loop => changeLoop(index, loop)} />
            ) : (
              <MacroAutocomplete
                macros={stepMacros}
                label={step.macroId > 0 && !macrosById[step.macroId] ? "Missing macro, choose another" : "Macro"}
                selected={macrosById[step.macroId] ?? null}
                allowCreateAndEdit={false}
                onSelect={(macro: MacroDefinition | null) => changeStep(index, { macroId: macro?.id ?? 0 })}
              />
            )}
          </Box>
          <Stack direction={"row"} flexShrink={0}>
            <Tooltip title={"Move up"}>
              <span>
                <IconButton size={"small"} disabled={index === 0} onClick={() => moveStep(index, -1)}>
                  <ArrowUpwardOutlinedIcon fontSize={"small"} />
                </IconButton>
              </span>
            </Tooltip>
            <Tooltip title={"Move down"}>
              <span>
                <IconButton
                  size={"small"}
                  disabled={index === steps.length - 1}
                  onClick={() => moveStep(index, 1)}
                >
                  <ArrowDownwardOutlinedIcon fontSize={"small"} />
                </IconButton>
              </span>
            </Tooltip>
            <Tooltip title={"Remove step"}>
              <IconButton size={"small"} onClick={() => removeStep(index)}>
                <DeleteOutlineOutlinedIcon fontSize={"small"} />
              </IconButton>
            </Tooltip>
          </Stack>
        </Stack>
      ))}
      <Stack direction={"row"} gap={1}>
        <Button startIcon={<AddOutlinedIcon />} onClick={addStep} disabled={steps.length >= MaxSequenceSteps}>
          Add Step
        </Button>
        <Button
          startIcon={<RepeatOutlinedIcon />}
          onClick={addLoop}
          disabled={steps.length === 0 || steps.length >= MaxSequenceSteps}
        >
          Add Loop
        </Button>
      </Stack>
    </Stack>
  )
}

// Changes the steps that the loops go back to, keeping the other steps as they are
function retargetLoops(steps: SequenceStep[], retarget: (target: number) => number): SequenceStep[] {
  return steps.map(step => step.loop ? { ...step, loop: { ...step.loop, targetStep: retarget(step.loop.targetStep) } } : step)
}

/**
 * The step a loop goes back to, which can only be one of the steps before it, and how many times it goes back. A loop
 * that was moved above the step it goes back to is shown as an error until another step is chosen.
 */
const LoopFields = (props: {
  index: number
  value: SequenceLoop
  onChange: (value: SequenceLoop) => void
}) => {
  const { index, value, onChange } = props

  const isValidTarget = value.targetStep < index

  const handleRepeatsChange = useCallback((e: ChangeEvent<HTMLInputElement>) => {
    const parsed = parseInt(e.target.value, 10)
    onChange({ ...value, repeats: Number.isNaN(parsed) ? 1 : Math.min(Math.max(parsed, 1), MaxSequenceLoopRepeats) })
  }, [value, onChange])

  return (
    <Stack direction={"row"} gap={1}>
      <TextField
        select
        label={isValidTarget ? "Go back to step" : "Choose an earlier step"}
        variant={"filled"}
        size={"small"}
        error={!isValidTarget}
        value={isValidTarget ? value.targetStep : ""}
        onChange={e => onChange({ ...value, targetStep: Number(e.target.value) })}
        sx={{ flex: 1, minWidth: 0 }}
      >
        {Array.from({ length: index }, (_, target) => (
          <MenuItem key={target} value={target}>{target + 1}</MenuItem>
        ))}
      </TextField>
      <TextField
        label={"Times"}
        variant={"filled"}
        size={"small"}
        type={"number"}
        value={value.repeats}
        onChange={handleRepeatsChange}
        sx={{ width: 120, flexShrink: 0 }}
        slotProps={{
          htmlInput: { min: 1, max: MaxSequenceLoopRepeats },
          input: { endAdornment: <InputAdornment position={"end"}>×</InputAdornment> },
        }}
      />
    </Stack>
  )
}

const DelayField = (props: {
  value: number
  onChange: (value: number) => void
}) => {
  const { value, onChange } = props

  const handleChange = useCallback((e: ChangeEvent<HTMLInputElement>) => {
    const parsed = parseInt(e.target.value, 10)
    onChange(Number.isNaN(parsed) ? 0 : Math.min(Math.max(parsed, 0), MaxSequenceStepDelayMs))
  }, [onChange])

  return (
    <TextField
      label={"Wait before"}
      variant={"filled"}
      size={"small"}
      type={"number"}
      value={value}
      onChange={handleChange}
      sx={{ width: 140, flexShrink: 0 }}
      slotProps={{
        htmlInput: { min: 0, max: MaxSequenceStepDelayMs, step: 50 },
        input: { endAdornment: <InputAdornment position={"end"}>ms</InputAdornment> },
      }}
    />
  )
}
