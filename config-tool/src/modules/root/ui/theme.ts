import { createTheme } from "@mui/material"

/**
 * Light and dark color schemes, as CSS variables, so that the CSS modules can use the palette too, e.g.
 * `var(--mui-palette-text-secondary)`. The selected scheme is set as `data-mui-color-scheme` on the root element, which
 * the app's own color tokens in index.css follow. MUI keeps the selected mode in local storage.
 */
export const theme = createTheme({
  cssVariables: {
    colorSchemeSelector: "data-mui-color-scheme",
  },
  colorSchemes: {
    light: {
      palette: {
        background: {
          default: "#eeeeee",
        },
      },
    },
    dark: true,
  },
})
