import DarkModeOutlinedIcon from "@mui/icons-material/DarkModeOutlined"
import LightModeOutlinedIcon from "@mui/icons-material/LightModeOutlined"
import { IconButton, Tooltip, useColorScheme } from "@mui/material"

/**
 * Switches between the light and dark color schemes. Follows the system until it's used the first time, after which the
 * selected scheme is kept.
 */
export const ColorSchemeToggle = () => {
  const { mode, systemMode, setMode } = useColorScheme()

  // Not known before the theme provider has read the stored mode
  if (!mode) {
    return null
  }

  const isDark = (mode === "system" ? systemMode : mode) === "dark"

  return (
    <Tooltip title={isDark ? "Switch to light mode" : "Switch to dark mode"}>
      <IconButton onClick={() => setMode(isDark ? "light" : "dark")}>
        {isDark ? <LightModeOutlinedIcon /> : <DarkModeOutlinedIcon />}
      </IconButton>
    </Tooltip>
  )
}
