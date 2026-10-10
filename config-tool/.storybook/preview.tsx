import { CssBaseline } from "@mui/material"
import type { Preview } from "@storybook/react-vite"

import "@fontsource/roboto/300.css"
import "@fontsource/roboto/400.css"
import "@fontsource/roboto/500.css"
import "@fontsource/roboto/700.css"

import "../src/index.css"

const preview: Preview = {
  decorators: [
    (Story) => (
      <>
        <CssBaseline />
        <Story />
      </>
    ),
  ],
  parameters: {
    controls: {
      matchers: {
        date: /Date$/i,
      },
    },
  },
}

export default preview
