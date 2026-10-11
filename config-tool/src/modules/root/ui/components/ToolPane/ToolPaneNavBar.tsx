import { Box, Divider, List, ListItem, ListItemButton, ListItemIcon, Paper, Tooltip, Typography } from "@mui/material"
import { AllAppModules } from "@src/AllAppModules"
import { useToolPaneContext } from "@src/modules/root/ui/components/ToolPane/ToolPaneContext"
import { Fragment } from "react"

import classes from "./ToolPaneNavBar.module.css"

export const ToolPaneNavBar = () => {

  const { selectedToolId, setSelectedToolId } = useToolPaneContext()

  const toolPanes = AllAppModules.flatMap(it => it.toolPanes)

  return (
    <Box sx={{ py: 2, pr: 2 }}>
      <Paper elevation={6} sx={{ borderRadius: 2, overflow: "hidden" }}>
        <List disablePadding>
          {toolPanes.map((it, index) => (
            <Fragment key={it.id}>
              {index > 0 && it.group !== toolPanes[index - 1].group && <Divider component={"li"} />}
              <ToolPaneNavItem
                title={it.title}
                shortTitle={it.shortTitle}
                icon={it.icon}
                selected={it.id === selectedToolId}
                onSelect={() => setSelectedToolId(it.id)}
              />
            </Fragment>
          ))}
        </List>
      </Paper>
    </Box>
  )
}

const ToolPaneNavItem = (props: {
  title: string
  shortTitle: string
  icon?: any
  selected: boolean
  onSelect: () => void
}) => {
  const { title, shortTitle, icon, selected, onSelect } = props

  return (
    <Tooltip title={title} arrow placement={"left"}>
      <ListItem disablePadding className={classes.ToolPaneNavItem}>
        <ListItemButton
          selected={selected}
          onClick={onSelect}
          sx={{ flexDirection: "column", width: 72, px: 0.5, py: 1 }}
        >
          <ListItemIcon sx={{
            justifyContent: "center",
            minWidth: 0,
          }}>
            {icon}
          </ListItemIcon>
          <Typography variant={"caption"}>
            {shortTitle}
          </Typography>
        </ListItemButton>
      </ListItem>
    </Tooltip>
  )
}
