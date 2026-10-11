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
  InputAdornment,
  List,
  ListItemButton,
  ListItemIcon,
  ListItemText,
  ListSubheader,
  Stack,
  TextField,
  Typography
} from "@mui/material"
import FolderOutlinedIcon from "@mui/icons-material/FolderOutlined"
import { CommandButton } from "@src/modules/common/components"
import { SaveMacroCommand } from "@src/modules/macros/commands"
import { useStoredMacrosQuery } from "@src/modules/macros/hooks"
import { macroTypeDefinitions } from "@src/modules/macros/macro-type-definitions"
import { type MacroDefinition, MacroDefinitionType, MacroTypeCategory } from "@src/modules/macros/models"
import { MacroTypeVisualization } from "@src/modules/macros/ui"
import { type ChangeEvent, type ComponentProps, memo, useCallback, useEffect, useMemo, useState } from "react"

const EmptyArray: any[] = []

export type EditMacroDialogProps = {
  macro?: MacroDefinition | null
  isOpen: boolean
  onClose: () => void
  // Receives the saved macro, with the ID the device assigned to it when it's new, before the dialog is closed
  onSaved?: (macro: MacroDefinition) => void
}

export const EditMacroDialog = (props: EditMacroDialogProps) => {
  const { onSaved, onClose } = props
  const state = useEditMacroDialogState(props)

  const handleSaved = useCallback((macro: MacroDefinition) => {
    onSaved?.(macro)
    onClose()
  }, [onSaved, onClose])

  return (
    <Dialog open={props.isOpen} onClose={state.handleClose} fullWidth maxWidth={"lg"}>
      <DialogTitle>{state.title}</DialogTitle>
      <Divider />
      {/*
        The types in a sidebar, next to the rest of the macro. Only the sides scroll, and the dialog keeps its height
        when switching between types, so that the sidebar doesn't move. It shrinks in windows too short for it.
      */}
      <Stack direction={"row"} sx={{ minHeight: 0, flex: "0 1 640px" }}>
        <Box sx={{ width: 240, flexShrink: 0, overflowY: "auto", borderRight: 1, borderColor: "divider" }}>
          <MacroTypeSelector
            value={state.macroDefinition.type}
            onChange={state.handleMacroTypeChange}
          />
        </Box>
        <DialogContent>
          <Stack gap={2}>
            <Stack direction={"row"} gap={2}>
              <MacroDirectoryEditor
                value={state.macroDefinition.directory}
                onChange={state.handleMacroDirectoryChange}
              />
              <MacroNameEditor
                value={state.macroDefinition.name}
                onChange={state.handleMacroNameChange}
              />
            </Stack>
            {state.willOverwriteExistingMacro && (
              <Alert severity={"warning"}>
                There is already a macro named <strong>{state.macroDefinition.name}</strong>.
                Saving will overwrite that macro.
              </Alert>
            )}
            <Divider />
            <MacroDefinitionEditor
              value={state.macroDefinition}
              onChange={state.handleMacroDefinitionChange}
            />
          </Stack>
        </DialogContent>
      </Stack>
      <Divider />
      <DialogActions>
        <Button onClick={props.onClose}>Cancel</Button>
        <CommandButton
          command={SaveMacroCommand}
          args={{ macro: state.macroDefinition }}
          onSuccess={handleSaved}
          variant={"contained"}
        />
      </DialogActions>
    </Dialog>
  )
}

type EditMacroDialogState = {
  title: string
  macroDefinition: MacroDefinition
  willOverwriteExistingMacro: boolean
  handleMacroDirectoryChange: (directory: string | null) => void
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

  const initialMacro = macro ?? createDefaultMacroDefinition(MacroDefinitionType.Shortcut, "New Macro")

  const [macroDefinition, setMacroDefinition] = useState<MacroDefinition>(initialMacro)

  const handleMacroDirectoryChange = useCallback((newDirectory: string | null) => {
    setMacroDefinition(s => ({ ...s, directory: newDirectory }))
  }, [setMacroDefinition])

  const handleMacroNameChange = useCallback((newName: string) => {
    setMacroDefinition(s => ({ ...s, name: newName }))
  }, [setMacroDefinition])

  const handleMacroTypeChange = useCallback((newType: MacroDefinitionType) => {
    if (macroDefinition.type !== newType) {
      setMacroDefinition(s => createDefaultMacroDefinition(newType, s.name))
    }
  }, [setMacroDefinition, macroDefinition.type])

  const storedMacros = useStoredMacrosQuery().data ?? EmptyArray

  const willOverwriteExistingMacro = useMemo(
    () => storedMacros.map(it => it.id).includes(macroDefinition.name) && macroDefinition.name !== macro?.name,
    [storedMacros, macroDefinition.name, macro]
  )

  const title = macroDefinition.id < 1 ? "Create Macro" : "Edit Macro"

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
    handleMacroDirectoryChange: handleMacroDirectoryChange,
    handleMacroNameChange: handleMacroNameChange,
    handleMacroTypeChange: handleMacroTypeChange,
    handleMacroDefinitionChange: setMacroDefinition,
    handleClose: handleClose,
  }
}

function createDefaultMacroDefinition(type: MacroDefinitionType, name: string): MacroDefinition {
  const typeDefinition = macroTypeDefinitions.byType[type]
  if (typeDefinition) {
    return typeDefinition.createDefaultMacroDefinition(name)
  }

  throw new Error(`Unsupported type: ${type}`)
}

const MacroDirectoryEditor = (props: {
  value: string | null
  onChange: (value: string | null) => void
}) => {
  const { value, onChange } = props

  // Suggests the directories already in use, while still allowing a new one to be typed
  const storedMacros = useStoredMacrosQuery().data ?? EmptyArray
  const directories = useMemo(
    () => [...new Set(storedMacros.map(it => it.directory).filter(it => it))].toSorted((a, b) => a.localeCompare(b)),
    [storedMacros]
  )

  const handleInputChange = useCallback((_: unknown, newValue: string) => {
    onChange(newValue.length > 0 ? newValue : null)
  }, [onChange])

  return (
    <Autocomplete
      freeSolo
      options={directories}
      inputValue={value ?? ""}
      onInputChange={handleInputChange}
      style={{ width: 240 }}
      renderInput={params => (
        <TextField
          {...params}
          label={"Directory"}
          // Macros without a directory are listed under the root
          placeholder={"<root>"}
          variant={"filled"}
          slotProps={{
            input: {
              ...params.InputProps,
              // Without the top margin of filled inputs, as the autocomplete already pads the input below the label
              startAdornment: (
                <InputAdornment position="start" style={{ marginTop: 0 }}>
                  <FolderOutlinedIcon fontSize={"small"} />
                </InputAdornment>
              )
            }
          }}
        />
      )}
    />
  )
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

// Lists the types by category, with the selected one highlighted
const MacroTypeSelector = memo((props: {
  value: MacroDefinitionType,
  onChange: (type: MacroDefinitionType) => void
}) => {
  const { value, onChange } = props

  return (
    <List dense>
      {Object.values(MacroTypeCategory).map(category => (
        <li key={category}>
          <ul style={{ padding: 0 }}>
            <ListSubheader>{category}</ListSubheader>
            {macroTypeDefinitions.all.filter(it => it.category === category).map(it => (
              <ListItemButton key={it.numericCode} selected={it.type === value} onClick={() => onChange(it.type)}>
                <ListItemIcon sx={{ minWidth: 36 }}>
                  <MacroTypeVisualization type={it.type} />
                </ListItemIcon>
                <ListItemText primary={it.displayName} />
              </ListItemButton>
            ))}
          </ul>
        </li>
      ))}
    </List>
  )
})

const MacroDefinitionEditor = (props: {
  value: MacroDefinition
  onChange: (value: MacroDefinition) => void
}) => {
  const { value, onChange } = props
  const type = value.type

  const typeDefinition = macroTypeDefinitions.byType[type]
  if (typeDefinition) {
    return (
      <>
        <Typography variant={"body2"} color={"text.secondary"}>{typeDefinition.description}</Typography>
        <typeDefinition.editorInformationComponent />
        <typeDefinition.editorComponent value={value} onChange={onChange} />
      </>
    )
  }

  return null
}
