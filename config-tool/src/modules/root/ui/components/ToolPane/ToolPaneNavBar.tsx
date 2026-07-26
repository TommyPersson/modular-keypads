import AssessmentOutlinedIcon from "@mui/icons-material/AssessmentOutlined"

import KeyboardAltOutlinedIcon from "@mui/icons-material/KeyboardAltOutlined"
import { Drawer, List, ListItem, ListItemButton, ListItemIcon, Toolbar, Tooltip } from "@mui/material"
import { DeviceLogsToolPaneIcon } from "@src/modules/device-debugger/ui"
import { useToolPaneContext } from "@src/modules/root/ui/components/ToolPane/ToolPaneContext"

import classes from "./ToolPaneNavBar.module.css"

export const ToolPaneNavBar = () => {

  const { selectedToolId, setSelectedToolId } = useToolPaneContext()

  return (
    <Drawer
      variant={"permanent"}
      anchor={"right"}
      sx={{
        width: 88,
        flexShrink: 0,
        [`& .MuiDrawer-paper`]: { width: 88, boxSizing: "border-box" },
      }}
    >
      <Toolbar />
      <List>
        <ToolPaneNavItem
          title={"Keyboard Events"}
          selected={selectedToolId === "keyboard-events"}
          onSelect={() => setSelectedToolId("keyboard-events")}
          icon={<KeyboardAltOutlinedIcon />}
        />
        <ToolPaneNavItem
          title={"Device Metrics"}
          selected={selectedToolId === "device-metrics"}
          onSelect={() => setSelectedToolId("device-metrics")}
          icon={<AssessmentOutlinedIcon />}
        />
        <ToolPaneNavItem
          title={"Device Logs"}
          selected={selectedToolId === "device-logs"}
          onSelect={() => setSelectedToolId("device-logs")}
          icon={<DeviceLogsToolPaneIcon />}
        />
      </List>
    </Drawer>
  )
}

const ToolPaneNavItem = (props: {
  title: string
  icon?: any
  selected: boolean
  onSelect: () => void
}) => {
  const { title, icon, selected, onSelect } = props

  return (
    <Tooltip title={title} arrow placement={"left"}>
      <ListItem disablePadding className={classes.ToolPaneNavItem}>
        <ListItemButton selected={selected} onClick={onSelect}>
          <ListItemIcon sx={{
            justifyContent: "center",
            mr: "auto",
          }}>
            {icon}
          </ListItemIcon>
        </ListItemButton>
      </ListItem>
    </Tooltip>
  )
}
