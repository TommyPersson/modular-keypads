import BugReportOutlinedIcon from "@mui/icons-material/BugReportOutlined"
import AssessmentOutlinedIcon from "@mui/icons-material/AssessmentOutlined"
import SwapVertOutlinedIcon from "@mui/icons-material/SwapVertOutlined"
import type { AppModule } from "@src/modules/common/AppModule"
import {
  DeviceDebuggerPage,
  DeviceLogsToolPaneContent,
  DeviceLogsToolPaneIcon,
  DeviceMetricsToolPaneContent,
  DeviceTrafficToolPaneContent
} from "@src/modules/device-debugger/ui"

export const DeviceDebuggerAppModule: AppModule = {
  id: "device-debugger",
  order: 999,
  navItems: [
    {
      title: "Device Debugger",
      link: "/device-debugger",
      icon: <BugReportOutlinedIcon />
    }
  ],
  routes: [
    {
      path: "device-debugger",
      Component: DeviceDebuggerPage,
    }
  ],
  toolPanes: [
    {
      id: "device-logs",
      title: "Device Logs",
      component: <DeviceLogsToolPaneContent />,
      icon: <DeviceLogsToolPaneIcon />
    },
    {
      id: "device-metrics",
      title: "Device Metrics",
      component: <DeviceMetricsToolPaneContent />,
      icon: <AssessmentOutlinedIcon />
    },
    {
      id: "device-traffic",
      title: "Device Serial Traffic",
      component: <DeviceTrafficToolPaneContent />,
      icon: <SwapVertOutlinedIcon />
    }
  ]
}