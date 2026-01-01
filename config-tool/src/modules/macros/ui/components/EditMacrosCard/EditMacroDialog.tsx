import {
  Alert,
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Divider,
  Grid,
  RadioGroup,
  Stack,
  TextField
} from "@mui/material"
import { CommandButton, RadioCard } from "@src/modules/common/components"
import { SaveMacroCommand } from "@src/modules/macros/commands"
import { useStoredMacrosQuery } from "@src/modules/macros/hooks"
import { macroTypeDefinitions } from "@src/modules/macros/macro-type-definitions"
import { type MacroDefinition, MacroDefinitionType } from "@src/modules/macros/models"
import { type ChangeEvent, type ComponentProps, memo, useCallback, useEffect, useMemo, useState } from "react"

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
            value={state.macroDefinition.name}
            onChange={state.handleMacroNameChange}
          />
          {state.willOverwriteExistingMacro && (
            <Alert severity={"warning"}>
              There is already a macro named <strong>{state.macroDefinition.name}</strong>.
              Saving will overwrite that macro.
            </Alert>
          )}
        </Stack>
      </DialogContent>
      <Divider />
      <DialogContent>
        <MacroTypeSelector
          value={state.macroDefinition.type}
          onChange={state.handleMacroTypeChange}
        />
      </DialogContent>
      <Divider />
      <DialogContent>
        <Stack gap={2}>
          <MacroDefinitionEditor
            value={state.macroDefinition}
            onChange={state.handleMacroDefinitionChange}
          />
        </Stack>
      </DialogContent>
      <DialogActions>
        <Button onClick={props.onClose}>Cancel</Button>
        <CommandButton
          command={SaveMacroCommand}
          args={{ macro: state.macroDefinition }}
          onSuccess={props.onClose}
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
        {macroTypeDefinitions.all.map(it => (
          <Grid size={4} key={it.numericCode}>
            <RadioCard
              label={it.displayName}
              value={it.type}
              onClick={onChange}
              description={it.description}
              fillHeight
            />
          </Grid>
        ))}
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

  const typeDefinition = macroTypeDefinitions.byType[type]
  if (typeDefinition) {
    return (
      <>
        <typeDefinition.editorInformationComponent />
        <typeDefinition.editorComponent value={value} onChange={onChange} />
      </>
    )
  }

  return null
}
