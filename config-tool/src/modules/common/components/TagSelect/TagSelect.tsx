import { Checkbox, Chip, ListItemText, MenuItem, Select, type SelectChangeEvent, Stack } from "@mui/material"
import { useCallback, type MouseEvent, type ComponentProps, useRef } from "react"

export const TagSelect = (props: {
  label?: string
  labelId?: string
  values: string[]
  chipSize?: ComponentProps<typeof Chip>["size"]
  availableValues: string[]
  onChange: (values: string[]) => void
}) => {

  const { values, availableValues, onChange } = props

  const selectRef = useRef<HTMLElement>(null)

  const handleChange = useCallback((e: SelectChangeEvent<string[]>) => {
    const fieldValue = e.target.value

    const newValues = typeof fieldValue === "string"
      ? fieldValue.split(",")
      : fieldValue

    onChange(newValues)
  }, [onChange])

  const handleTagDeleted = useCallback((deleted: string) => {
    const newValues = values.filter(it => it !== deleted)
    onChange(newValues)
  }, [values, onChange])

  return (
    <Select
      labelId={props.labelId}
      label={props.label}
      multiple
      value={values}
      onChange={handleChange}
      renderValue={(modifiers) => {
        return (
          <Stack direction={"row"} spacing={0.5}>
            {modifiers.map(modifier => (
              <TagItem value={modifier} onDelete={handleTagDeleted} size={props.chipSize} />
            ))}
          </Stack>
        )
      }}
      ref={selectRef}
    >
      {availableValues.map(value => (
        <MenuItem key={value} value={value} sx={{ height: 48 }}>
          <Checkbox checked={values.includes(value as any)} />
          <ListItemText>{value}</ListItemText>
        </MenuItem>
      ))}
    </Select>
  )
}

const TagItem = (props: {
  value: string
  onDelete: (deleted: string) => void
  size?: ComponentProps<typeof Chip>["size"]
}) => {
  const { value, onDelete } = props

  const handleDelete = useCallback(() => {
    onDelete(value)
  }, [value, onDelete])

  const handleMouseDown = useCallback((e: MouseEvent<any>) => {
    // Without this the click event won't be passed to delete button
    // and will instead open the outer Select drop down.
    e.preventDefault()
    e.stopPropagation()
  }, [])

  return (
    <Chip
      size={props.size}
      key={value}
      label={value}
      onDelete={handleDelete}
      onMouseDown={handleMouseDown}
    />
  )
}