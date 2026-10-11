import { TextField } from "@mui/material"
import { type ChangeEvent, useCallback, useEffect, useState } from "react"

// Limits of the signed byte values in the HID report
const MIN_VALUE = -127
const MAX_VALUE = 127

// Incomplete input, such as "" or "-", counts as zero
const parseValue = (text: string) => {
  const parsed = parseInt(text, 10)
  return isNaN(parsed) ? 0 : Math.min(MAX_VALUE, Math.max(MIN_VALUE, parsed))
}

// Keeps the typed text, so that intermediate input such as "-" isn't replaced while typing
export const SignedByteField = (props: {
  label: string
  helperText: string
  value: number
  onChange: (value: number) => void
}) => {
  const { label, helperText, value, onChange } = props

  const [text, setText] = useState(value.toString())

  useEffect(() => {
    setText(current => parseValue(current) === value ? current : value.toString())
  }, [value])

  const handleChange = useCallback((event: ChangeEvent<HTMLInputElement>) => {
    const newText = event.target.value
    setText(newText)
    onChange(parseValue(newText))
  }, [onChange])

  return (
    <TextField
      variant={"filled"}
      type={"number"}
      label={label}
      helperText={helperText}
      value={text}
      onChange={handleChange}
      onBlur={() => setText(value.toString())}
      slotProps={{ htmlInput: { min: MIN_VALUE, max: MAX_VALUE, step: 1 } }}
      fullWidth
    />
  )
}
