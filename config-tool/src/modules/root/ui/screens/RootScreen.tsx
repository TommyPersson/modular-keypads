import { Alert, AlertTitle, Box, Button, Drawer } from "@mui/material"
import { useDeviceContext } from "@src/modules/device/context"
import { useCallback } from "react"
import { DeviceDebuggerPage } from "../../../device-debugger/ui"
import { MainAppBar } from "../components"

import classes from "./RootScreen.module.css"

export const RootScreen = () => {

  const deviceContext = useDeviceContext()
  const { facade: deviceFacade, isConnected } = deviceContext

  const handleConnectClick = useCallback(() => {
    deviceFacade.connect().then()
  }, [deviceFacade])

  return (
    <Box className={classes.RootScreen}>
      <MainAppBar />

      <Drawer/>

      {!isConnected && (
        <Alert severity={"error"}>
          <AlertTitle>Not connected!</AlertTitle>
          <Button onClick={handleConnectClick}>Connect</Button>
        </Alert>
      )}

      <DeviceDebuggerPage />
    </Box>
  )
}