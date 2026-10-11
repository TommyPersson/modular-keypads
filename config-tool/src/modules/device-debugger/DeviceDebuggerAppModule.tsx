import AssessmentOutlinedIcon from "@mui/icons-material/AssessmentOutlined"
import SwapVertOutlinedIcon from "@mui/icons-material/SwapVertOutlined"
import type { AppModule } from "@src/modules/common/AppModule"
import {
  DeviceLogsToolPaneContent,
  DeviceLogsToolPaneIcon,
  DeviceMetricsToolPaneContent,
  DeviceTrafficToolPaneContent
} from "@src/modules/device-debugger/ui"

export const DeviceDebuggerAppModule: AppModule = {
  id: "device-debugger",
  order: 999,
  routes: [],
  toolPanes: [
    {
      id: "device-logs",
      title: "Device Logs",
      shortTitle: "Logs",
      group: "debug",
      component: <DeviceLogsToolPaneContent />,
      icon: <DeviceLogsToolPaneIcon />
    },
    {
      id: "device-metrics",
      title: "Device Metrics",
      shortTitle: "Metrics",
      group: "debug",
      component: <DeviceMetricsToolPaneContent />,
      icon: <AssessmentOutlinedIcon />
    },
    {
      id: "device-traffic",
      title: "Device Serial Traffic",
      shortTitle: "Traffic",
      group: "debug",
      component: <DeviceTrafficToolPaneContent />,
      icon: <SwapVertOutlinedIcon />
    }
  ]
}