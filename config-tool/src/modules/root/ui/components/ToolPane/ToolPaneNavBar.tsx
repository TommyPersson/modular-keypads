import { Drawer, List, ListItem, ListItemButton, ListItemIcon, Toolbar, Tooltip } from "@mui/material"
import { AllAppModules } from "@src/AllAppModules"
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
        {...AllAppModules
          .flatMap(it => it.toolPanes)
          .map(it =>
            <ToolPaneNavItem
              key={it.id}
              title={it.title}
              icon={it.icon}
              selected={it.id === selectedToolId}
              onSelect={() => setSelectedToolId(it.id)}
            />
          )
        }
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
