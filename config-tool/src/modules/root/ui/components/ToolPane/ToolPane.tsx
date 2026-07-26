import { Paper } from "@mui/material"
import { DeviceLogsToolPaneContent } from "@src/modules/device-debugger/ui"
import { DeviceMetricsToolPaneContent } from "@src/modules/device-debugger/ui/components/DeviceMetricsToolPaneContent"
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
  switch (toolId) {
    case 'keyboard-events':
      return <KeyboardEventsToolPaneContent />
    case 'device-metrics':
      return <DeviceMetricsToolPaneContent />
    case 'device-logs':
      return <DeviceLogsToolPaneContent />
    default:
      return null
  }
}