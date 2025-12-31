import { Autocomplete, Box, FormControl, Stack, TextField } from "@mui/material"
import { consumerControlCodes } from "@src/modules/key-bindings/data/consumerControlCodes"
import type { ConsumerControlCode, ConsumerControlMacroDefinition } from "@src/modules/key-bindings/models"
import { useCallback, useMemo } from "react"

export const ConsumerControlMacroDefinitionEditor = (props: {
  value: ConsumerControlMacroDefinition
  onChange: (value: ConsumerControlMacroDefinition) => void
}) => {
  const { value, onChange } = props

  const code = value?.usageId ?? null

  const handleCodeChange = useCallback((usageId: number | null) => {
    onChange({ ...value, usageId: usageId ?? 0 })
  }, [value, onChange])

  return <ConsumerControlCodeEditor value={code} onChange={handleCodeChange} />
}

export const ConsumerControlCodeEditor = (props: {
  value: number | null
  onChange: (value: number | null) => void
}) => {
  const { value, onChange } = props

  const selectedCode = consumerControlCodes.byUsageId[value ?? 0] ?? null

  const sortedCodes = useMemo(() => {
    return consumerControlCodes.usable
      .toSorted((a, b) => a.section.localeCompare(b.section))
  }, [])

  const handleCodeChange = useCallback((_: any, code: ConsumerControlCode | null) => {
    onChange(code?.usageId ?? null)
  }, [onChange])

  return (
    <FormControl sx={{ minWidth: 400 }} variant={"filled"}>
      <Autocomplete
        renderInput={(params) => (
          <TextField variant={"filled"} {...params} label="Consumer Control Code" />
        )}
        renderOption={(props, option) => (
          <Box component={"li"} {...props} key={props.key}>
            <ConsumerControlCodeItem code={option} />
          </Box>
        )}
        renderValue={(option) => (
          <ConsumerControlCodeItem code={option} />
        )}
        options={sortedCodes}
        getOptionLabel={option => option.usageName + option.section + `0x${option.usageId.toString(16).padStart(2, "0")}`}
        groupBy={option => option.section}
        onChange={handleCodeChange}
        value={selectedCode}
      />
    </FormControl>
  )
}

export const ConsumerControlCodeItem = (props: {
  code: ConsumerControlCode
}) => {
  const { code } = props
  return (
    <Stack direction={"row"} gap={2} width={"100%"} alignItems={"baseline"}>
      <code>0x{code.usageId.toString(16).padStart(2, "0")}</code>
      <strong>{code.usageName}</strong>
    </Stack>
  )
}
