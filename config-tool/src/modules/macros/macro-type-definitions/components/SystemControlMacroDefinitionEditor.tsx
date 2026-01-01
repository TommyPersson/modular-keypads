import { Autocomplete, Box, FormControl, Stack, TextField } from "@mui/material"
import { systemControlCodes } from "@src/modules/macros/data/systemControlCodes"
import type { SystemControlCode, SystemControlMacroDefinition } from "@src/modules/macros/models"
import { useCallback } from "react"

export const SystemControlMacroDefinitionEditor = (props: {
  value: SystemControlMacroDefinition
  onChange: (value: SystemControlMacroDefinition) => void
}) => {
  const { value, onChange } = props

  const code = value?.code ?? null

  const handleCodeChange = useCallback((code: number | null) => {
    onChange({ ...value, code: code ?? 0 })
  }, [value, onChange])

  return <SystemControlCodeEditor value={code} onChange={handleCodeChange} />
}

export const SystemControlCodeEditor = (props: {
  value: number | null
  onChange: (value: number | null) => void
}) => {
  const { value, onChange } = props

  const selectedCode = systemControlCodes.byCode[value ?? 0] ?? null

  const handleCodeChange = useCallback((_: any, code: SystemControlCode | null) => {
    onChange(code?.code ?? null)
  }, [onChange])

  return (
    <FormControl sx={{ minWidth: 400 }} variant={"filled"}>
      <Autocomplete
        renderInput={(params) => (
          <TextField variant={"filled"} {...params} label="System Control Code" />
        )}
        renderOption={(props, option) => (
          <Box component={"li"} {...props} key={props.key}>
            <SystemControlCodeItem code={option} />
          </Box>
        )}
        renderValue={(option) => (
          <SystemControlCodeItem code={option} />
        )}
        options={systemControlCodes.all}
        getOptionLabel={option => option.description + `0x${option.code.toString(16).padStart(2, "0")}`}
        onChange={handleCodeChange}
        value={selectedCode}
      />
    </FormControl>
  )
}

export const SystemControlCodeItem = (props: {
  code: SystemControlCode
}) => {
  const { code } = props
  return (
    <Stack direction={"row"} gap={2} width={"100%"} alignItems={"baseline"}>
      <strong>{code.description}</strong>
    </Stack>
  )
}
