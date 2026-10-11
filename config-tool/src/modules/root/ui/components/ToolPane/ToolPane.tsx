import { Box, Collapse, Paper, Stack } from "@mui/material"
import { AllAppModules } from "@src/AllAppModules"
import {
  KeyboardEventsToolPaneContent
} from "@src/modules/key-bindings/ui/components/KeyboardEventsToolPaneContent/KeyboardEventsToolPaneContent"
import { useToolPaneContext } from "@src/modules/root/ui"
import { overlayInsets, TOOL_PANE_MARGIN, TOOL_PANE_WIDTH } from "@src/modules/root/ui/overlayLayout"
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
        {/* Ends above the main control bar, so that they don't overlap in narrow windows */}
        <Box
          flex={1}
          minHeight={0}
          padding={`${TOOL_PANE_MARGIN}px`}
          paddingBottom={`${overlayInsets(true).bottom}px`}
        >
          <Paper elevation={6} sx={{ height: "100%", width: TOOL_PANE_WIDTH, overflowY: "auto", borderRadius: 2 }}>
            {content}
          </Paper>
        </Box>
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