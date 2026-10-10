import ChevronRightOutlinedIcon from "@mui/icons-material/ChevronRightOutlined"
import {
  Autocomplete,
  type AutocompleteInputChangeReason,
  type AutocompleteValue,
  Box,
  Stack,
  TextField
} from "@mui/material"
import type { MacroDefinition } from "@src/modules/macros/models"
import { MacroTypeVisualization } from "@src/modules/macros/ui"
import { useCallback, useMemo } from "react"

const MacroOptionVisualization = (props: {
  macro: MacroDefinition
}) => {
  const { macro } = props

  return (
    <Stack direction={"row"} gap={2} alignItems={"baseline"}>
      <Stack direction={"row"} gap={1} alignItems={"center"}>
        <MacroTypeVisualization type={macro.type} />
        <strong>{macro.name}</strong>
      </Stack>
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
      getOptionLabel={option => option.name + option.id}
      value={selected}
      onChange={handleChange}
      onInputChange={handleInputChange}
      disabled={disabled}
    />
  )
}
