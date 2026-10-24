import { CssBaseline, ThemeProvider } from "@mui/material"
import { theme } from "../src/modules/root/ui/theme"
import type { Preview } from "@storybook/react-vite"
import { StrictMode } from "react"
import { ColorSchemeFromToolbar, type ColorSchemeMode } from "./ColorSchemeFromToolbar"

import "@fontsource/roboto/300.css"
import "@fontsource/roboto/400.css"
import "@fontsource/roboto/500.css"
import "@fontsource/roboto/700.css"

import "../src/index.css"

const preview: Preview = {
  decorators: [
    // In strict mode by default, as the app is, so that stories behave the same as in development. It can be turned off
    // in the toolbar to see how they behave in production, e.g. for transitions, which start over in strict mode when
    // their elements are moved. Turning it on or off mounts the story again.
    //
    // The color scheme is selected in the toolbar rather than kept in local storage, so that the toggle in the stories
    // only lasts until the story is shown again.
    (Story, context) => {
      const mode = context.globals.colorScheme as ColorSchemeMode
      const story = (
        <ThemeProvider theme={theme} storageManager={null} defaultMode={mode}>
          <ColorSchemeFromToolbar mode={mode} />
          <CssBaseline />
          <Story />
        </ThemeProvider>
      )

      return context.globals.strictMode === "off" ? story : <StrictMode>{story}</StrictMode>
    },
  ],
  globalTypes: {
    strictMode: {
      description: "Render the stories in React's strict mode",
      toolbar: {
        title: "Strict mode",
        icon: "lock",
        items: [
          { value: "on", title: "Strict mode on" },
          { value: "off", title: "Strict mode off" },
        ],
        dynamicTitle: true,
      },
    },
    colorScheme: {
      description: "The color scheme of the stories",
      toolbar: {
        title: "Color scheme",
        icon: "mirror",
        items: [
          { value: "system", title: "System color scheme" },
          { value: "light", title: "Light color scheme" },
          { value: "dark", title: "Dark color scheme" },
        ],
        dynamicTitle: true,
      },
    },
  },
  initialGlobals: {
    strictMode: "on",
    colorScheme: "system",
  },
  parameters: {
    controls: {
      matchers: {
        date: /Date$/i,
      },
    },
  },
}

export default preview
