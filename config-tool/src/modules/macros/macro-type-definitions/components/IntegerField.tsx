import { TextField } from "@mui/material"
import { type ChangeEvent, useCallback, useEffect, useState } from "react"
import { IntegerType } from "./IntegerType"

// The signed ranges leave out the lowest value, like the device does, which keeps them symmetric for movement and
// scrolling in both directions
const INTEGER_RANGES: Record<IntegerType, { min: number, max: number }> = {
  [IntegerType.S8]: { min: -127, max: 127 },
  [IntegerType.U8]: { min: 0, max: 255 },
  [IntegerType.S16]: { min: -32767, max: 32767 },
  [IntegerType.U16]: { min: 0, max: 65535 },
}

// Incomplete input, such as "" or "-", counts as zero
const parseValue = (text: string, min: number, max: number) => {
  const parsed = parseInt(text, 10)
  return isNaN(parsed) ? 0 : Math.min(max, Math.max(min, parsed))
}

// Keeps the typed text, so that intermediate input such as "-" isn't replaced while typing
export const IntegerField = (props: {
  label: string
  helperText: string
  type: IntegerType
  value: number
  onChange: (value: number) => void
}) => {
  const { label, helperText, type, value, onChange } = props
  const { min, max } = INTEGER_RANGES[type]

  const [text, setText] = useState(value.toString())

  useEffect(() => {
    setText(current => parseValue(current, min, max) === value ? current : value.toString())
  }, [value, min, max])

  const handleChange = useCallback((event: ChangeEvent<HTMLInputElement>) => {
    const newText = event.target.value
    setText(newText)
    onChange(parseValue(newText, min, max))
  }, [onChange, min, max])

  return (
    <TextField
      variant={"filled"}
      type={"number"}
      label={label}
      helperText={helperText}
      value={text}
      onChange={handleChange}
      onBlur={() => setText(value.toString())}
      slotProps={{ htmlInput: { min, max, step: 1 } }}
      fullWidth
    />
  )
}
