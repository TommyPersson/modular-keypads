import { InputAdornment, TextField } from "@mui/material"
import { type ChangeEvent, type ReactNode, useCallback, useEffect, useState } from "react"
import { IntegerType } from "./IntegerType"

// The signed ranges leave out the lowest value, like the device does, which keeps them symmetric for movement and
// scrolling in both directions
const INTEGER_RANGES: Record<IntegerType, { min: number, max: number }> = {
  [IntegerType.S8]: { min: -127, max: 127 },
  [IntegerType.U8]: { min: 0, max: 255 },
  [IntegerType.S16]: { min: -32767, max: 32767 },
  [IntegerType.U16]: { min: 0, max: 65535 },
}

// The input's right padding plus the width of the spin buttons, which Chromium draws inside the input element. The
// config tool only runs in Chromium, as it needs Web Serial.
const SPIN_BUTTONS_OFFSET_PX = 12 + 15

// Placed over the end of the input rather than after it, so that it's left of the spin buttons, which would otherwise
// sit between the number and its end content. Spans the filled input's vertical padding, so that it's centered on the
// number and the spin buttons rather than on the field with its label. Clicks pass through to the input.
const EndContent = (props: { children: ReactNode }) => (
  <InputAdornment
    position={"end"}
    sx={{
      position: "absolute",
      right: SPIN_BUTTONS_OFFSET_PX + 4,
      top: "25px",
      bottom: "8px",
      height: "auto",
      maxHeight: "none",
      pointerEvents: "none",
    }}
  >
    {props.children}
  </InputAdornment>
)

// Incomplete input, such as "" or "-", counts as zero
const parseValue = (text: string, min: number, max: number) => {
  const parsed = parseInt(text, 10)
  return isNaN(parsed) ? 0 : Math.min(max, Math.max(min, parsed))
}

// Keeps the typed text, so that intermediate input such as "-" isn't replaced while typing. The min and max narrow the
// range of the type, for values with limits of their own. The end content is shown at the end of the input, such as
// the name of the value.
export const IntegerField = (props: {
  label: string
  helperText: string
  type: IntegerType
  min?: number
  max?: number
  disabled?: boolean
  endContent?: ReactNode
  value: number
  onChange: (value: number) => void
}) => {
  const { label, helperText, type, disabled, endContent, value, onChange } = props
  const min = Math.max(INTEGER_RANGES[type].min, props.min ?? -Infinity)
  const max = Math.min(INTEGER_RANGES[type].max, props.max ?? Infinity)

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
      disabled={disabled}
      slotProps={{
        htmlInput: { min, max, step: 1 },
        input: endContent ? { endAdornment: <EndContent>{endContent}</EndContent> } : undefined,
      }}
      fullWidth
    />
  )
}
