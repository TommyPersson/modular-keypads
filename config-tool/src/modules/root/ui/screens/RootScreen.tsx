import { Box, CssBaseline, Fade, Stack } from "@mui/material"
import { useDeviceContext } from "@src/modules/device/context"
import { UpdateFirmwareDialog } from "@src/modules/device/ui"
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

      <Stack style={{ height: "100vh", overflow: "auto" }}>
        <Outlet />
      </Stack>

      {/*
        On top of the page rather than next to it, so that the page keeps the whole window. Only the panes themselves
        take the pointer, so that the page can still be used around them.
      */}
      <Stack direction={"row"} className={classes.ToolPaneOverlay}>
        <ToolPane />
        <Fade in={isConnected} unmountOnExit>
          <div>
            <ToolPaneNavBar />
          </div>
        </Fade>
      </Stack>

      <MainControlBar />

      {/* Here rather than with the device, as the device disconnects while it restarts into the new firmware */}
      <UpdateFirmwareDialog />
    </Box>
  )
}
