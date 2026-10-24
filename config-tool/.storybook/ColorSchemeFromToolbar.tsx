import { useColorScheme } from "@mui/material"
import { useEffect } from "react"

export type ColorSchemeMode = "system" | "light" | "dark"

/** Applies the mode selected in the toolbar, also when it's changed while a story is shown, without mounting it again */
export const ColorSchemeFromToolbar = (props: { mode: ColorSchemeMode }) => {
  const { setMode } = useColorScheme()
  useEffect(() => setMode(props.mode), [props.mode, setMode])
  return null
}
