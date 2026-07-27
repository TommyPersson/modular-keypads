import { Box, CssBaseline, Grid, Stack, Toolbar } from "@mui/material"
import { useDeviceContext } from "@src/modules/device/context"
import { ToolPane } from "@src/modules/root/ui/components/ToolPane"
import { ToolPaneNavBar } from "@src/modules/root/ui/components/ToolPane/ToolPaneNavBar"
import { useCallback } from "react"
import { Outlet } from "react-router"
import { MainAppBar, MainNavBar } from "../components"

import classes from "./RootScreen.module.css"

export const RootScreen = () => {

  const deviceContext = useDeviceContext()
  const { facade: deviceFacade, isConnected } = deviceContext

  const handleConnectClick = useCallback(() => {
    deviceFacade.connect().then()
  }, [deviceFacade])

  return (
    <Box className={classes.RootScreen}>
      <CssBaseline />

      <MainAppBar />

      <Grid container>
        <Grid size={"auto"}>
          <MainNavBar
            isConnected={isConnected}
            onConnectClick={handleConnectClick}
          />
        </Grid>
        <Grid size={"grow"}>
          <Stack style={{ height: "100vh", overflow: "hidden" }}>
            <Toolbar />
            <Stack flexGrow={1} overflow={"auto"}>
              <Outlet />
            </Stack>
          </Stack>
        </Grid>
        <Grid size={"auto"} style={{ height: "100vh" }}>
          <Stack flexGrow={1} overflow={"auto"} style={{ height: "100vh" }}>
            <Toolbar />
            <ToolPane />
          </Stack>
        </Grid>
        <Grid size={"auto"}>
          <ToolPaneNavBar />
        </Grid>
      </Grid>
    </Box>
  )
}
