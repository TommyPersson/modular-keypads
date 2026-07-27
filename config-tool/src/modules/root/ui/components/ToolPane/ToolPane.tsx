import { Paper } from "@mui/material"
import { AllAppModules } from "@src/AllAppModules"
import {
  KeyboardEventsToolPaneContent
} from "@src/modules/key-bindings/ui/components/KeyboardEventsToolPaneContent/KeyboardEventsToolPaneContent"
import { useToolPaneContext } from "@src/modules/root/ui"

export const ToolPane = () => {

  const { selectedToolId } = useToolPaneContext()

  const content = renderContent(selectedToolId)

  if (!content) {
    return null
  }

  return (
    <Paper style={{ height: "100%" }}>
      {content}
    </Paper>
  )
}

function renderContent(toolId: string | null) {
  const toolPanes = AllAppModules.flatMap(it => it.toolPanes)
  const toolPane = toolPanes.find(it => it.id === toolId)
  if (toolPane?.component) {
    return toolPane.component
  }

  switch (toolId) {
    case 'keyboard-events':
      return <KeyboardEventsToolPaneContent />
    default:
      return null
  }
}