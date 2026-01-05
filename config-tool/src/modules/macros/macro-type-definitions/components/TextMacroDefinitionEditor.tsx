import { TextField } from "@mui/material"
import type { TextMacroDefinition } from "@src/modules/macros/models"
import { type ChangeEvent, useCallback } from "react"

export const TextMacroDefinitionEditor = (props: {
  value: TextMacroDefinition
  onChange: (value: TextMacroDefinition) => void
}) => {
  const { value, onChange } = props

  const text = value.text

  const handleTextChange = useCallback((text: string | null) => {
    onChange({ ...value, text: text ?? "" })
  }, [value, onChange])

  return <TextEditor value={text} onChange={handleTextChange} />
}

export const TextEditor = (props: {
  value: string | null
  onChange: (value: string | null) => void
}) => {
  const { value, onChange } = props

  const handleValueChange = useCallback((event: ChangeEvent<HTMLTextAreaElement>) => {
    onChange(event.target.value)
  }, [onChange])

  return (
    <TextField
      label="Text"
      multiline
      rows={4}
      value={value ?? ""}
      onChange={handleValueChange}
      defaultValue=""
    />
  )
}