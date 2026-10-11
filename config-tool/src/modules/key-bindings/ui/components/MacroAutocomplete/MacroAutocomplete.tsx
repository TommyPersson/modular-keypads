import AddOutlinedIcon from "@mui/icons-material/AddOutlined"
import ChevronRightOutlinedIcon from "@mui/icons-material/ChevronRightOutlined"
import EditOutlinedIcon from "@mui/icons-material/EditOutlined"
import {
  Autocomplete,
  type AutocompleteInputChangeReason,
  type AutocompleteValue,
  Box,
  IconButton,
  Stack,
  TextField,
  Tooltip
} from "@mui/material"
import { createMacroFilter } from "@src/modules/macros/macroFilter"
import type { MacroDefinition } from "@src/modules/macros/models"
import { EditMacroDialog, MacroDefinitionVisualization, MacroTypeVisualization } from "@src/modules/macros/ui"
import { Children, cloneElement, type ReactElement, type ReactNode, useCallback, useMemo, useState } from "react"

// The name takes a fixed part of the width, so that the definitions line up between the options
const MacroOptionVisualization = (props: {
  macro: MacroDefinition
}) => {
  const { macro } = props

  return (
    <Stack direction={"row"} gap={2} alignItems={"center"} width={"100%"} minWidth={0}>
      <Stack direction={"row"} gap={1} alignItems={"center"} flex={"0 0 40%"} minWidth={0}>
        <MacroTypeVisualization type={macro.type} />
        <strong style={{ whiteSpace: "nowrap", overflow: "hidden", textOverflow: "ellipsis" }}>{macro.name}</strong>
      </Stack>
      <Box flex={1} minWidth={0} overflow={"hidden"}>
        <MacroDefinitionVisualization macro={macro} />
      </Box>
    </Stack>
  )
}

const MacroValueVisualization = (props: {
  macro: MacroDefinition | null
}) => {
  const { macro } = props

  if (!macro) {
    return
  }

  return (
    <Stack direction={"row"} gap={2} alignItems={"center"} flex={1} style={{ maxWidth: 400 }}>
      <Stack direction={"row"} gap={0.5} alignItems={"center"} flex={1} style={{ minWidth: 0, paddingLeft: 4 }}>
        <span style={{ whiteSpace: "nowrap", overflow: "hidden", textOverflow: "ellipsis" }}>
          {macro.directory ?? "<root>"}
        </span>
        <ChevronRightOutlinedIcon />
        <MacroTypeVisualization type={macro.type} />
        <strong style={{ whiteSpace: "nowrap", overflow: "hidden", textOverflow: "ellipsis" }}>{macro.name}</strong>
      </Stack>
    </Stack>
  )
}

/**
 * Selects one of the macros, or a new one, which is created in the macro dialog and selected once it's saved. The
 * selected macro can be edited in the same dialog, from a button next to the clear button.
 */
export const MacroAutocomplete = (props: {
  macros: MacroDefinition[]
  label: string
  selected: MacroDefinition | null
  disabled?: boolean
  onSelect?: (macro: MacroDefinition | null) => void
}) => {
  const { macros, label, selected, disabled, onSelect } = props

  const options = useMemo(() => {
    return macros.toSorted((a, b) => (a.directory + a.name).localeCompare(b.directory + b.name))
  }, [macros])

  // The same filter as in the macros tool pane, so that macros can also be found by e.g. their keys
  const filterOptions = useCallback((options: MacroDefinition[], state: { inputValue: string }) => {
    const matches = createMacroFilter(state.inputValue)
    return matches === null ? options : options.filter(matches)
  }, [])

  const handleChange = useCallback((_: any, value: AutocompleteValue<MacroDefinition, unknown, unknown, unknown>) => {
    if (value !== null) {
      onSelect?.(value)
    }
  }, [onSelect])

  const handleInputChange = useCallback((_: any, __: any, reason: AutocompleteInputChangeReason) => {
    if (reason === "clear") {
      onSelect?.(null)
    }
  }, [onSelect])

  // The macro being edited, or null when creating one. Kept while the dialog closes, so that it doesn't change then.
  const [isDialogOpen, setIsDialogOpen] = useState(false)
  const [dialogMacro, setDialogMacro] = useState<MacroDefinition | null>(null)

  const openDialog = useCallback((macro: MacroDefinition | null) => {
    setDialogMacro(macro)
    setIsDialogOpen(true)
  }, [])

  const handleSaved = useCallback((macro: MacroDefinition) => {
    // An edited macro keeps its ID, so it's still the one selected
    if (dialogMacro === null) {
      onSelect?.(macro)
    }
  }, [dialogMacro, onSelect])

  const editButton = selected && (
    <Tooltip title={"Edit macro"} key={"edit"}>
      <IconButton
        className={EDIT_BUTTON_CLASS}
        size={"small"}
        sx={{ p: "4px" }}
        disabled={disabled}
        onClick={() => openDialog(selected)}
      >
        <EditOutlinedIcon fontSize={"small"} />
      </IconButton>
    </Tooltip>
  )

  return (
    <Stack direction={"row"} alignItems={"center"} gap={1}>
      <Autocomplete
        sx={editButtonOnHover}
        size={"small"}
        renderInput={(params) => (
          <TextField
            variant={"filled"}
            {...params}
            label={label}
            slotProps={{
              input: {
                ...params.InputProps,
                endAdornment: withLeadingChild(params.InputProps.endAdornment, editButton),
              },
            }}
          />
        )}
        renderOption={(props, option) => (
          <Box component={"li"} {...props} key={props.key}>
            <MacroOptionVisualization macro={option} />
          </Box>
        )}
        renderValue={(option) => (
          <MacroValueVisualization macro={option} />
        )}
        groupBy={it => it.directory ?? "<root>"}
        options={options}
        filterOptions={filterOptions}
        getOptionLabel={option => option.name + option.id}
        value={selected}
        onChange={handleChange}
        onInputChange={handleInputChange}
        disabled={disabled}
      />
      <Tooltip title={"Create a new macro for this action"}>
        <span>
          <IconButton onClick={() => openDialog(null)} disabled={disabled}>
            <AddOutlinedIcon />
          </IconButton>
        </span>
      </Tooltip>
      <EditMacroDialog
        macro={dialogMacro}
        isOpen={isDialogOpen}
        onClose={() => setIsDialogOpen(false)}
        onSaved={handleSaved}
      />
    </Stack>
  )
}

const EDIT_BUTTON_CLASS = "macro-edit-button"

// Shown like the clear button, while hovered or focused, and with room for it at the end of the input
const editButtonOnHover = {
  flex: 1,
  [`& .${EDIT_BUTTON_CLASS}`]: { visibility: "hidden" },
  [`&:hover .${EDIT_BUTTON_CLASS}, &.Mui-focused .${EDIT_BUTTON_CLASS}, &:focus-within .${EDIT_BUTTON_CLASS}`]: {
    visibility: "visible",
  },
  "&.MuiAutocomplete-hasClearIcon .MuiAutocomplete-inputRoot.MuiAutocomplete-inputRoot": { paddingRight: "93px" },
}

// Adds the child before the children of the autocomplete's end adornment, which holds its clear and popup buttons
function withLeadingChild(endAdornment: ReactNode, child: ReactNode) {
  if (!child) {
    return endAdornment
  }

  const element = endAdornment as ReactElement<{ children?: ReactNode }>
  return cloneElement(element, undefined, child, ...Children.toArray(element.props.children))
}
