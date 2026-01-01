import RadioButtonCheckedOutlinedIcon from "@mui/icons-material/RadioButtonCheckedOutlined"
import StopCircleOutlinedIcon from "@mui/icons-material/StopCircleOutlined"
import {
  Autocomplete,
  Box,
  Button,
  Fade,
  FormControl,
  InputLabel,
  Paper,
  Popper,
  Stack,
  TextField,
  useTheme
} from "@mui/material"
import { TagSelect } from "@src/modules/common/components"
import { keyboadKeyCodes } from "@src/modules/macros/data"
import type { KeyboardKeyCode, Shortcut, ShortcutMacroDefinition } from "@src/modules/macros/models"
import { type ComponentProps, useCallback, useEffect, useState } from "react"

import classes from "./ShortcutMacroDefinitionEditor.module.css"

export const ShortcutMacroDefinitionEditor = (props: {
  value: ShortcutMacroDefinition
  onChange: (value: ShortcutMacroDefinition) => void
}) => {
  const { value, onChange } = props

  const shortcut = value.shortcut

  const handleShortcutChange = useCallback((shortcut: Shortcut) => {
    onChange({ ...value, shortcut: shortcut })
  }, [value, onChange])

  return <ShortcutEditor value={shortcut} onChange={handleShortcutChange} />
}

export const ShortcutEditor = (props: {
  value: Shortcut
  onChange: (value: Shortcut) => void
}) => {
  const { value, onChange } = props

  const selectedModifiers = value.modifiers

  const handleModifiersChange = useCallback((values: number[]) => {
    onChange({
      ...value,
      modifiers: values
    })
  }, [value, onChange])

  const keyCode = value.hidCode > 0
    ? keyboadKeyCodes.byHidCode[value.hidCode] ?? null
    : null

  const handleKeyChange = useCallback((keyCode: KeyboardKeyCode | null) => {
    onChange({
      ...value,
      hidCode: keyCode?.hidCode ?? 0
    })
  }, [value, onChange])

  const handleRecorded = useCallback((shortcut: Shortcut) => {
    onChange(shortcut)
  }, [onChange])

  return (
    <Stack direction={"row"} spacing={2}>
      <ModifiersEditor values={selectedModifiers} onChange={handleModifiersChange} />
      <KeyboardKeyCodeEditor value={keyCode} onChange={handleKeyChange} />
      <RecordShortcutButton onRecorded={handleRecorded} />
    </Stack>
  )
}

export const ModifiersEditor = (props: {
  values: number[]
  onChange: (values: number[]) => void
}) => {
  const { values, onChange } = props

  const availableModifiers = keyboadKeyCodes.modifiers
  const modifierNames = availableModifiers.map(it => it.hidDescription)
  const selectedModifierNames = values.map(hidCode =>
    keyboadKeyCodes.modifiers.find(it => it.hidCode === hidCode)
  ).filter(it => it).map(it => it!.hidDescription!)

  const handleModifiersChange = useCallback((values: string[]) => {
    const keyCodes = values.map(tagValue =>
      keyboadKeyCodes.modifiers.find(it => it.hidDescription === tagValue)
    ).filter(it => it).map(it => it!.hidCode)

    onChange(keyCodes)
  }, [onChange])

  return (
    <FormControl sx={{ minWidth: 400 }} variant={"filled"}>
      <InputLabel>Modifiers</InputLabel>
      <TagSelect
        label={"Modifiers"}
        values={selectedModifierNames}
        availableValues={modifierNames}
        onChange={handleModifiersChange}
        chipSize={"small"}
      />
    </FormControl>
  )
}

export const KeyboardKeyCodeEditor = (props: {
  value: KeyboardKeyCode | null
  onChange: (value: KeyboardKeyCode | null) => void
}) => {
  const { value, onChange } = props

  const sortedHidCodes = keyboadKeyCodes.all
    .filter(it => !it.hidden)
    .toSorted((a, b) => a.category!.localeCompare(b.category!))

  const handleKeyChange = useCallback((_: any, keyCode: KeyboardKeyCode | null) => {
    onChange(keyCode)
  }, [onChange])

  return (
    <FormControl sx={{ minWidth: 400 }} variant={"filled"}>
      <Autocomplete
        renderInput={(params) => (
          <TextField variant={"filled"} {...params} label="HID Key" />
        )}
        renderOption={(props, option) => (
          <Box component={"li"} {...props} key={props.key}>
            <KeyboardKeyCodeItem keyCode={option} />
          </Box>
        )}
        renderValue={(option) => (
          <KeyboardKeyCodeItem keyCode={option} />
        )}
        options={sortedHidCodes}
        getOptionLabel={option => option.hidDescription + option.jsCode}
        groupBy={option => option.category ?? "Other"}
        onChange={handleKeyChange}
        value={value}
      />
    </FormControl>
  )
}

export const KeyboardKeyCodeItem = (props: {
  keyCode: KeyboardKeyCode
}) => {
  const { keyCode } = props
  return (
    <Stack direction={"row"} gap={2} width={"100%"} alignItems={"baseline"}>
      <code>0x{keyCode.hidCode.toString(16).padStart(2, "0")}</code>
      <strong>{keyCode.hidDescription}</strong>
      <div style={{ flex: 1 }} />
      <code>{keyCode.jsCode ?? "N/A"}</code>
    </Stack>
  )
}

export const RecordShortcutButton = (props: {
  onRecorded: (shortcut: Shortcut) => void
}) => {
  const { onRecorded } = props

  const [isRecording, setIsRecording] = useState(false)
  const [buttonRef, setButtonRef] = useState<HTMLElement | null>(null)

  const icon = isRecording
    ? <StopCircleOutlinedIcon />
    : <RadioButtonCheckedOutlinedIcon />

  const label = isRecording
    ? "Cancel Recording"
    : "Record Key Press"

  const color: ComponentProps<typeof Button>["color"] = isRecording
    ? "error"
    : "error"

  const handleClick = useCallback(() => {
    setIsRecording(s => !s)
  }, [setIsRecording])

  useEffect(() => {
    const pressedModifierKeys = new Set<number>()

    const keyDownHandler = (e: KeyboardEvent) => {
      if (!isRecording) {
        return
      }

      e.preventDefault()
      e.stopPropagation()
      e.stopImmediatePropagation()

      const keyCode = keyboadKeyCodes.byJsCode[e.code]
      if (keyCode.isModifier) {
        pressedModifierKeys.add(keyCode.hidCode)
      } else {
        onRecorded({
          modifiers: Array.from(pressedModifierKeys),
          hidCode: keyCode.hidCode
        })
        setIsRecording(false)
      }
    }

    const keyUpHandler = (e: KeyboardEvent) => {
      if (!isRecording) {
        return
      }

      e.preventDefault()
      e.stopPropagation()
      e.stopImmediatePropagation()

      const keyCode = keyboadKeyCodes.byJsCode[e.code]
      if (keyCode.isModifier) {
        pressedModifierKeys.delete(keyCode.hidCode)
      }
    }

    document.addEventListener("keydown", keyDownHandler)
    document.addEventListener("keyup", keyUpHandler)

    return () => {
      document.removeEventListener("keydown", keyDownHandler)
      document.removeEventListener("keyup", keyUpHandler)
    }
  }, [isRecording, setIsRecording, onRecorded])

  const theme = useTheme()

  return (
    <>
      <Button
        startIcon={icon}
        color={color}
        onClick={handleClick}
        variant={"contained"}
        data-recording={isRecording}
        ref={setButtonRef}
        className={classes.RecordButton}
      >
        {label}
      </Button>
      <Popper
        id={"test"}
        open={isRecording}
        anchorEl={buttonRef}
        placement={"bottom"}
        transition
        style={{ zIndex: theme.zIndex.modal + 1 }}
      >
        {({ TransitionProps }) => (
          <Fade {...TransitionProps} timeout={350}>
            <Box maxWidth={500} sx={{ margin: 1 }}>
              <Paper sx={{ padding: 2, bgcolor: "info.main", color: "info.contrastText" }}>
                Keep in mind that not all key combinations can be captured reliably due to browser behaviors.
              </Paper>
            </Box>
          </Fade>
        )}
      </Popper>
    </>
  )
}
