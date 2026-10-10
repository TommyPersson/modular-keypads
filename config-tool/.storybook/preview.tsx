import { CssBaseline } from "@mui/material"
import type { Preview } from "@storybook/react-vite"
import { StrictMode } from "react"

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
    (Story, context) => {
      const story = (
        <>
          <CssBaseline />
          <Story />
        </>
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
  },
  initialGlobals: {
    strictMode: "on",
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
