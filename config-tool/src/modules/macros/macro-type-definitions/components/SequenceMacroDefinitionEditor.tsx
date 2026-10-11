import AddOutlinedIcon from "@mui/icons-material/AddOutlined"
import ArrowDownwardOutlinedIcon from "@mui/icons-material/ArrowDownwardOutlined"
import ArrowUpwardOutlinedIcon from "@mui/icons-material/ArrowUpwardOutlined"
import DeleteOutlineOutlinedIcon from "@mui/icons-material/DeleteOutlineOutlined"
import { Box, Button, IconButton, InputAdornment, Stack, TextField, Tooltip, Typography } from "@mui/material"
import { useStoredMacros } from "@src/modules/macros/hooks"
import {
  type MacroDefinition,
  MacroDefinitionType,
  MaxSequenceStepDelayMs,
  MaxSequenceSteps,
  type SequenceMacroDefinition,
  type SequenceStep
} from "@src/modules/macros/models"
import { MacroAutocomplete } from "@src/modules/macros/ui/components/MacroAutocomplete"
import { type ChangeEvent, useCallback, useMemo } from "react"

/**
 * Edits the steps of a sequence, each a macro and the time to wait before running it. The steps can be moved up and
 * down, and other sequences can't be chosen as steps.
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

  const moveStep = (index: number, offset: number) => {
    const newSteps = [...steps]
    const [step] = newSteps.splice(index, 1)
    newSteps.splice(index + offset, 0, step)
    setSteps(newSteps)
  }

  const removeStep = (index: number) => {
    setSteps(steps.filter((_, i) => i !== index))
  }

  const addStep = () => {
    setSteps([...steps, { macroId: 0, delayMs: 0 }])
  }

  return (
    <Stack gap={1.5}>
      {steps.map((step, index) => (
        // Keyed by the macro too, so that a moved step's fields are recreated with its values
        <Stack key={`${index}:${step.macroId}`} direction={"row"} gap={1} alignItems={"center"}>
          <Typography color={"text.secondary"} sx={{ width: 24, flexShrink: 0, textAlign: "right" }}>
            {index + 1}.
          </Typography>
          <DelayField value={step.delayMs} onChange={delayMs => changeStep(index, { delayMs })} />
          <Box flex={1} minWidth={0}>
            <MacroAutocomplete
              macros={stepMacros}
              label={step.macroId > 0 && !macrosById[step.macroId] ? "Missing macro, choose another" : "Macro"}
              selected={macrosById[step.macroId] ?? null}
              allowCreateAndEdit={false}
              onSelect={(macro: MacroDefinition | null) => changeStep(index, { macroId: macro?.id ?? 0 })}
            />
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
      <Box>
        <Button startIcon={<AddOutlinedIcon />} onClick={addStep} disabled={steps.length >= MaxSequenceSteps}>
          Add Step
        </Button>
      </Box>
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
