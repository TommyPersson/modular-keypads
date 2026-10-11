import { Box, CssBaseline, Fade, Grid, Stack } from "@mui/material"
import { useDeviceContext } from "@src/modules/device/context"
import { MainControlBar } from "@src/modules/root/ui/components/MainControlBar"
import { ToolPane, useToolPaneContext } from "@src/modules/root/ui/components/ToolPane"
import { ToolPaneNavBar } from "@src/modules/root/ui/components/ToolPane/ToolPaneNavBar"
import { useQueryClient } from "@tanstack/react-query"
import { useEffect, useRef } from "react"
import { Outlet } from "react-router"

import classes from "./RootScreen.module.css"

export const RootScreen = () => {

  const { isConnected } = useDeviceContext()
  const queryClient = useQueryClient()

  // Only when the connection changes, as nothing has been loaded from the device before that
  const wasConnected = useRef(isConnected)
  useEffect(() => {
    if (isConnected !== wasConnected.current) {
      wasConnected.current = isConnected
      queryClient.resetQueries().then() // TODO give all device queries a prefix
    }
  }, [isConnected, queryClient])

  // The tool panes all need a connected device
  const { setSelectedToolId } = useToolPaneContext()
  useEffect(() => {
    if (!isConnected) {
      setSelectedToolId(null)
    }
  }, [isConnected, setSelectedToolId])

  return (
    <Box className={classes.RootScreen}>
      <CssBaseline />

      <Grid container>
        <Grid size={"grow"}>
          <Stack style={{ height: "100vh", overflow: "hidden" }}>
            <Stack flexGrow={1} overflow={"auto"}>
              <Outlet />
            </Stack>
          </Stack>
        </Grid>
        <Grid size={"auto"} style={{ height: "100vh" }}>
          <Stack flexGrow={1} overflow={"auto"} style={{ height: "100vh" }}>
            <ToolPane />
          </Stack>
        </Grid>
        <Grid size={"auto"}>
          <Fade in={isConnected} unmountOnExit>
            <div>
              <ToolPaneNavBar />
            </div>
          </Fade>
        </Grid>
      </Grid>

      <MainControlBar />
    </Box>
  )
}
