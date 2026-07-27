import {
  Alert,
  AlertTitle,
  Button,
  Drawer,
  List,
  ListItem,
  ListItemButton,
  ListItemIcon,
  ListItemText,
  Toolbar
} from "@mui/material"
import { AllAppModules } from "@src/AllAppModules"
import type { NavItemDefinition } from "@src/modules/root/ui/components/MainNavBar/NavItemDefinition"
import { NavLink, useMatch } from "react-router"

import classes from "./MainNavBar.module.css"

export const MainNavBar = (props: {
  isConnected: boolean
  onConnectClick: () => void
}) => {
  const { isConnected, onConnectClick } = props

  const drawerWidth = 300

  return (
    <Drawer
      variant={"permanent"}
      sx={{
        width: 300,
        flexShrink: 0,
        [`& .MuiDrawer-paper`]: { width: drawerWidth, boxSizing: "border-box" },
      }}
    >
      <Toolbar />
      {!isConnected && (
        <Alert severity={"error"}>
          <AlertTitle>Not connected!</AlertTitle>
          <Button onClick={onConnectClick}>Connect</Button>
        </Alert>
      )}
      <List>
        {...AllAppModules
          .flatMap(it => it.navItems)
          .map(it => <NavItem key={it.link} {...it} />)}
      </List>
    </Drawer>
  )
}

export const NavItem = (props: {} & NavItemDefinition) => {
  const { link, title, icon } = props

  const isSelected = !!useMatch(`${link}/*`)

  return (
    <ListItem disablePadding className={classes.NavItem}>
      <NavLink to={link}>
        <ListItemButton selected={isSelected}>
          <ListItemIcon>
            {icon}
          </ListItemIcon>
          <ListItemText>
            {title}
          </ListItemText>
        </ListItemButton>
      </NavLink>
    </ListItem>
  )
}