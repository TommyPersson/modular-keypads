import { StrictMode } from "react"
import { createRoot } from "react-dom/client"

import "@fontsource/roboto/300.css"
import "@fontsource/roboto/400.css"
import "@fontsource/roboto/500.css"
import "@fontsource/roboto/700.css"

import { App } from "./App.tsx"
import { isStrictModeEnabled } from "./utils/strictMode"

import "./index.css"

const strictMode = isStrictModeEnabled()
if (!strictMode && import.meta.env.DEV) {
  console.info("Strict mode is off, turn it on again with ?strictMode=on")
}

createRoot(document.getElementById("root")!).render(
  strictMode
    ? <StrictMode><App /></StrictMode>
    : <App />,
)
