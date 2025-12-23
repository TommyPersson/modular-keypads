import { Stack } from "@mui/material"

export type PageProps = {
  children: any
}

export const Page = (props: PageProps) => {
  const { children } = props

  return (
    <Stack padding={2}>
      {children}
    </Stack>
  )
}