import RadioButtonCheckedOutlinedIcon from "@mui/icons-material/RadioButtonCheckedOutlined"
import StopCircleOutlinedIcon from "@mui/icons-material/StopCircleOutlined"

import {
  Alert,
  Autocomplete,
  Box,
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Divider,
  Fade,
  FormControl,
  Grid,
  InputLabel,
  Paper,
  Popper,
  RadioGroup,
  Stack,
  TextField,
  useTheme
} from "@mui/material"
import { RadioCard, TagSelect } from "@src/modules/common/components"
import { jsModifierKeyNames, keyboadKeyCodes } from "@src/modules/key-bindings/data"
import { useStoredMacrosQuery } from "@src/modules/key-bindings/hooks"
import {
  type KeyboardKeyCode,
  type MacroDefinition,
  MacroDefinitionType,
  ModifierKey,
  type Shortcut,
  type ShortcutMacroDefinition
} from "@src/modules/key-bindings/models"
import { type ChangeEvent, type ComponentProps, memo, useCallback, useEffect, useMemo, useState } from "react"

import classes from "./EditMacroDialog.module.css"

const EmptyArray: any[] = []

export type EditMacroDialogProps = {
  macro?: MacroDefinition | null
  isOpen: boolean
  onClose: () => void
}

export const EditMacroDialog = (props: EditMacroDialogProps) => {
  const state = useEditMacroDialogState(props)

  return (
    <Dialog open={props.isOpen} onClose={state.handleClose} fullWidth maxWidth={"lg"}>
      <DialogTitle>{state.title}</DialogTitle>
      <DialogContent>
        <Stack gap={2}>
          <MacroNameEditor
            value={state.macroDefinition.id}
            onChange={state.handleMacroNameChange}
          />
          {state.willOverwriteExistingMacro && (
            <Alert severity={"warning"}>
              There is already a macro named <strong>{state.macroDefinition.id}</strong>.
              Saving will overwrite that macro.
            </Alert>
          )}
        </Stack>
      </DialogContent>
      <Divider />
      <DialogContent>
        <MacroTypeSelector
          value={state.macroDefinition.type}
          onChange={state.handleMacroTypeChange} />
      </DialogContent>
      <Divider />
      <DialogContent>
        <Stack gap={2}>
          <Alert severity={"info"}>
            Keep in mind that the keys shown in HID code list are the raw HID descriptions. The values has not been
            updated to match the current locale. For example, the <code>Q</code> key should be selected instead of
            <code>A</code> in AZERTY layouts.
          </Alert>
          <MacroDefinitionEditor
            value={state.macroDefinition}
            onChange={state.handleMacroDefinitionChange}
          />
        </Stack>
      </DialogContent>
      <DialogActions>
        <Button onClick={props.onClose}>Cancel</Button>
        <Button variant={"contained"} disabled={!state.canSave}>Save</Button>
      </DialogActions>
    </Dialog>
  )
}

type EditMacroDialogState = {
  title: string
  macroDefinition: MacroDefinition
  willOverwriteExistingMacro: boolean
  canSave: boolean
  handleMacroNameChange: (name: string) => void
  handleMacroTypeChange: (type: MacroDefinitionType) => void
  handleMacroDefinitionChange: (definition: MacroDefinition) => void
  handleClose: ComponentProps<typeof Dialog>["onClose"]
}

function useEditMacroDialogState(props: EditMacroDialogProps): EditMacroDialogState {
  const { macro, isOpen, onClose } = props

  // eslint-disable-next-line react-hooks/exhaustive-deps
  const handleClose = useCallback(((_, reason) => {
    if (reason === "backdropClick") {
      return
    }

    onClose()
  }) satisfies ComponentProps<typeof Dialog>["onClose"], [onClose])

  const initialMacro = props.macro ?? createDefaultMacroDefinition(MacroDefinitionType.Shortcut, "New Macro")

  const [macroDefinition, setMacroDefinition] = useState<MacroDefinition>(initialMacro)

  const handleMacroNameChange = useCallback((newName: string) => {
    setMacroDefinition(s => ({ ...s, id: newName }))
  }, [setMacroDefinition])

  const handleMacroTypeChange = useCallback((newType: MacroDefinitionType) => {
    if (macroDefinition.type !== newType) {
      setMacroDefinition(s => createDefaultMacroDefinition(newType, s.id))
    }
  }, [setMacroDefinition, macroDefinition.type])

  const storedMacros = useStoredMacrosQuery().data ?? EmptyArray

  const willOverwriteExistingMacro = useMemo(
    () => storedMacros.map(it => it.id).includes(macroDefinition.id) && macroDefinition.id !== macro?.id,
    [storedMacros, macroDefinition.id, macro]
  )
  const canSave = canSaveMacro(macroDefinition)

  const title = macroDefinition.isNew ? "Create Macro" : "Edit Macro"

  useEffect(() => {
    if (isOpen && macro) {
      setMacroDefinition(macro)
    } else {
      setMacroDefinition(createDefaultMacroDefinition(MacroDefinitionType.Shortcut, "New Macro"))
    }
  }, [isOpen, macro, setMacroDefinition])

  return {
    title: title,
    macroDefinition: macroDefinition,
    willOverwriteExistingMacro: willOverwriteExistingMacro,
    canSave: canSave,
    handleMacroNameChange: handleMacroNameChange,
    handleMacroTypeChange: handleMacroTypeChange,
    handleMacroDefinitionChange: setMacroDefinition,
    handleClose: handleClose,
  }
}

function canSaveMacro(macro: MacroDefinition) {
  switch (macro.type) {
    case MacroDefinitionType.Shortcut:
      return macro.shortcut.hidCode > 0
    default:
      return false
  }
}

function createDefaultMacroDefinition(type: MacroDefinitionType, name: string) {
  switch (type) {
    case MacroDefinitionType.Shortcut:
      return {
        id: name,
        type: MacroDefinitionType.Shortcut,
        shortcut: {
          modifiers: [],
          hidCode: 0
        },
        isNew: true
      } satisfies ShortcutMacroDefinition
    default:
      throw new Error(`Unsupported type: ${type}`)
  }
}

const MacroNameEditor = (props: {
  value: string
  onChange: (value: string) => void
}) => {
  const { value, onChange } = props

  const handleOnChange = useCallback((e: ChangeEvent<HTMLInputElement>) => {
    onChange(e.target.value)
  }, [onChange])

  return (
    <TextField
      label={"Name"}
      variant={"filled"}
      helperText={"No more than 32 characters"}
      value={value}
      onChange={handleOnChange}
      style={{ width: 400 }}
    />
  )
}

const MacroTypeSelector = memo((props: {
  value: MacroDefinitionType,
  onChange: (type: MacroDefinitionType) => void
}) => {
  const { value, onChange } = props

  const handleTypeChange = useCallback((e: ChangeEvent<HTMLInputElement>) => {
    onChange(e.target.value as MacroDefinitionType)
  }, [onChange])

  return (
    <RadioGroup
      value={value}
      onChange={handleTypeChange}
    >
      <Grid container spacing={2}>
        <Grid size={4}>
          <RadioCard
            label={"Shortcut"}
            value={MacroDefinitionType.Shortcut}
            onClick={onChange}
            description={
              <>
                A <strong>Shortcut</strong> is a single regular key press, possibly combined with
                modifiers.
              </>
            }
            fillHeight
          />
        </Grid>
        <Grid size={4}>
          <RadioCard
            label={"Shortcut Sequence"}
            value={MacroDefinitionType.ShortcutSequence}
            onClick={onChange}
            description={
              <>A series of <strong>Shortcuts</strong> executed in sequence.</>
            }
            fillHeight
            disabled
          />
        </Grid>
        <Grid size={4}>
          <RadioCard
            label={"HID Sequence"}
            value={MacroDefinitionType.HIDKeySequence}
            onClick={onChange}
            description={
              <>A series of raw <strong>HID events</strong> executed in sequence.</>
            }
            fillHeight
            disabled
          />
        </Grid>
      </Grid>
    </RadioGroup>
  )
})

const MacroDefinitionEditor = (props: {
  value: MacroDefinition
  onChange: (value: MacroDefinition) => void
}) => {
  const { value, onChange } = props
  const type = value.type

  switch (type) {
    case "Shortcut":
      return <ShortcutMacroDefinitionEditor value={value} onChange={onChange} />
    default:
      return null // TODO
  }
}

const ShortcutMacroDefinitionEditor = (props: {
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
const ShortcutEditor = (props: {
  value: Shortcut
  onChange: (value: Shortcut) => void
}) => {
  const { value, onChange } = props

  const selectedModifiers = value.modifiers

  const handleModifiersChange = useCallback((values: ModifierKey[]) => {
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

const ModifiersEditor = (props: {
  values: ModifierKey[]
  onChange: (values: ModifierKey[]) => void
}) => {
  const { values, onChange } = props

  const handleModifiersChange = useCallback((values: string[]) => {
    onChange(values as ModifierKey[])
  }, [onChange])

  const availableModifiers = Object.keys(ModifierKey)

  return (
    <FormControl sx={{ minWidth: 400 }} variant={"filled"}>
      <InputLabel>Modifiers</InputLabel>
      <TagSelect
        label={"Modifiers"}
        values={values}
        availableValues={availableModifiers}
        onChange={handleModifiersChange}
        chipSize={"small"}
      />
    </FormControl>
  )
}

const KeyboardKeyCodeEditor = (props: {
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
          <Box component={"li"} {...props}>
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

const KeyboardKeyCodeItem = (props: {
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

const RecordShortcutButton = (props: {
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
    const keyDownHandler = (e: KeyboardEvent) => {
      if (!isRecording) {
        return
      }

      e.preventDefault()
      e.stopPropagation()
      e.stopImmediatePropagation()

      const shortcut = getShortcutFromKeyEvent(e)
      if (shortcut) {
        onRecorded(shortcut)
        setIsRecording(false)
      }
    }

    document.addEventListener("keydown", keyDownHandler)

    return () => {
      document.removeEventListener("keydown", keyDownHandler)
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

function getShortcutFromKeyEvent(e: KeyboardEvent): Shortcut | null {
  if (e.code.length === 0 || e.repeat) {
    return null
  }

  if (jsModifierKeyNames.includes(e.key)) {
    return null
  }

  const hidCode = keyboadKeyCodes.byJsCode[e.code]?.hidCode
  if (!hidCode) {
    return null
  }

  const modifiers = [
    e.altKey ? ModifierKey.Alt : null,
    e.ctrlKey ? ModifierKey.Control : null,
    e.metaKey ? ModifierKey.Meta : null,
    e.shiftKey ? ModifierKey.Shift : null,
  ].filter(it => it) as ModifierKey[]

  return {
    modifiers,
    hidCode
  }
}