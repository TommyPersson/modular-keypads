import BugReportOutlinedIcon from "@mui/icons-material/BugReportOutlined"
import HomeOutlinedIcon from "@mui/icons-material/HomeOutlined"
import KeyboardOutlinedIcon from '@mui/icons-material/KeyboardOutlined';

import {
  Alert,
  AlertTitle,
  Box,
  Button,
  CssBaseline, Divider,
  Drawer,
  Grid,
  List,
  ListItem,
  ListItemButton,
  ListItemIcon,
  ListItemText,
  Stack,
  Toolbar
} from "@mui/material"
import { useDeviceContext } from "@src/modules/device/context"
import { useCallback } from "react"
import { NavLink, Outlet, useMatch } from "react-router"
import { MainAppBar } from "../components"

import classes from "./RootScreen.module.css"

export const RootScreen = () => {

  const deviceContext = useDeviceContext()
  const { facade: deviceFacade, isConnected } = deviceContext

  const handleConnectClick = useCallback(() => {
    deviceFacade.connect().then()
  }, [deviceFacade])

  const drawerWidth = 300

  return (
    <Box className={classes.RootScreen}>
      <CssBaseline />

      <MainAppBar />

      <Grid container>
        <Grid size={"auto"}>
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
                <Button onClick={handleConnectClick}>Connect</Button>
              </Alert>
            )}
            <List>
              <NavItem
                title={"Overview"}
                link={"/overview"}
                icon={<HomeOutlinedIcon />}
              />
              <NavItem
                title={"Key Bindings"}
                link={"/key-bindings"}
                icon={<KeyboardOutlinedIcon />}
              />
              <Divider />
              <NavItem
                title={"Device Debugger"}
                link={"/device-debugger"}
                icon={<BugReportOutlinedIcon />}
              />
            </List>
          </Drawer>
        </Grid>
        <Grid size={"grow"}>
          <Stack style={{ height: "100vh", overflow: "hidden" }}>
            <Toolbar />
            <Stack flexGrow={1} overflow={"auto"}>
              <Outlet />
            </Stack>
          </Stack>
        </Grid>
      </Grid>
    </Box>
  )
}
const NavItem = (props: {
  link: string
  title: string
  icon?: any
}) => {
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