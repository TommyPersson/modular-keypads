import ChevronRightOutlinedIcon from "@mui/icons-material/ChevronRightOutlined"
import {
  Autocomplete,
  type AutocompleteInputChangeReason,
  type AutocompleteValue,
  Box,
  Stack,
  TextField
} from "@mui/material"
import { createMacroFilter } from "@src/modules/macros/macroFilter"
import type { MacroDefinition } from "@src/modules/macros/models"
import { MacroDefinitionVisualization, MacroTypeVisualization } from "@src/modules/macros/ui"
import { useCallback, useMemo } from "react"

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
    onSelect?.(value)
  }, [onSelect])

  const handleInputChange = useCallback((_: any, __: any, reason: AutocompleteInputChangeReason) => {
    if (reason === "clear") {
      onSelect?.(null)
    }
  }, [onSelect])

  return (
    <Autocomplete
      size={"small"}
      renderInput={(params) => (
        <TextField variant={"filled"} {...params} label={label} />
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
  )
}
