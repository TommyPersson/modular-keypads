import { Collapse, Paper, Stack, Toolbar } from "@mui/material"
import { AllAppModules } from "@src/AllAppModules"
import {
  KeyboardEventsToolPaneContent
} from "@src/modules/key-bindings/ui/components/KeyboardEventsToolPaneContent/KeyboardEventsToolPaneContent"
import { useToolPaneContext } from "@src/modules/root/ui"
import { useState } from "react"

export const ToolPane = () => {

  const { selectedToolId } = useToolPaneContext()

  // Keep rendering the last selected tool while the pane is collapsing
  const [lastToolId, setLastToolId] = useState(selectedToolId)
  if (selectedToolId !== null && selectedToolId !== lastToolId) {
    setLastToolId(selectedToolId)
  }

  const isOpen = selectedToolId !== null
  const content = renderContent(selectedToolId ?? lastToolId)

  return (
    <Collapse in={isOpen} orientation={"horizontal"} unmountOnExit style={{ height: "100%" }}>
      <Stack style={{ height: "100vh" }}>
        <Toolbar />
        <Paper style={{ height: "100%", width: 700, overflowY: "auto" }}>
          {content}
        </Paper>
      </Stack>
    </Collapse>
  )
}

function renderContent(toolId: string | null) {
  const toolPanes = AllAppModules.flatMap(it => it.toolPanes)
  const toolPane = toolPanes.find(it => it.id === toolId)
  if (toolPane?.component) {
    return toolPane.component
  }

  switch (toolId) {
    case "keyboard-events":
      return <KeyboardEventsToolPaneContent />
    default:
      return null
  }
}