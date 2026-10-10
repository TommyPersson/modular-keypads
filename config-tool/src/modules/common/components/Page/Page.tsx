import { Stack } from "@mui/material"

export type PageProps = {
  children: any
  noPadding?: boolean
  // Fills the height available to the page, so that its content can be sized to it rather than to their content
  fillHeight?: boolean
}

export const Page = (props: PageProps) => {
  const { children, noPadding = false, fillHeight = false } = props

  return (
    <Stack
      padding={noPadding ? 0 : 2}
      sx={fillHeight ? { flexGrow: 1, minHeight: 0 } : undefined}
    >
      {children}
    </Stack>
  )
}