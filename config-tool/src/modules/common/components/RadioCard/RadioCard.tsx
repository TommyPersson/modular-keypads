import { Card, CardActionArea, CardContent, FormControlLabel, Radio, Stack, Typography } from "@mui/material"
import type { Theme } from "@mui/material/styles"
import type { SxProps } from "@mui/system"
import { type CSSProperties, useCallback } from "react"

export const RadioCard = (props: {
  label: any
  description?: any
  value: any
  style?: CSSProperties
  fillHeight?: boolean
  disabled?: boolean
  onClick?: (value: any) => void
}) => {

  const { label, description, value, disabled, onClick } = props

  const handleClick = useCallback(() => {
    if (!disabled) {
      onClick?.(value)
    }
  }, [onClick, value, disabled])

  const style: CSSProperties = {
    height: props.fillHeight ? "100%" : undefined,
    ...props.style,
  }

  const descriptionStyle: SxProps<Theme> = {
    marginLeft: 4,
    color: disabled ? "text.disabled" : undefined
  }

  return (
    <Card onClick={handleClick} style={style}>
      <CardActionArea style={{ height: style.height }} disabled={disabled}>
        <CardContent style={{ height: style.height }}>
          <Stack style={{ height: style.height }}>
            <FormControlLabel
              value={value}
              control={<Radio />}
              label={label}
              disabled={disabled}
            />
            <Typography variant={"body2"} sx={descriptionStyle}>
              {description}
            </Typography>
            <div style={{ flex: 1 }} />
          </Stack>
        </CardContent>
      </CardActionArea>
    </Card>
  )
}