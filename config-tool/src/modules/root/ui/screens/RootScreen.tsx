import { Box, CssBaseline, Grid, Stack } from "@mui/material"
import { useDeviceContext } from "@src/modules/device/context"
import { ToolPane } from "@src/modules/root/ui/components/ToolPane"
import { ToolPaneNavBar } from "@src/modules/root/ui/components/ToolPane/ToolPaneNavBar"
import { queryClient } from "@src/utils/queryClient"
import { useEffect } from "react"
import { Outlet } from "react-router"

import classes from "./RootScreen.module.css"

export const RootScreen = () => {

  const { isConnected } = useDeviceContext()

  useEffect(() => {
    queryClient.resetQueries().then() // TODO give all device queries a prefix
  }, [isConnected])

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
          <ToolPaneNavBar />
        </Grid>
      </Grid>
    </Box>
  )
}
