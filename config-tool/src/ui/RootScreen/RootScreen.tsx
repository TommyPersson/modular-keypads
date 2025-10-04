import { Alert, AlertTitle, Box, Button, Grid, Stack } from "@mui/material"
import { useCallback } from "react"
import { useDeviceContext } from "../../context/DeviceContext"
import { DeviceInformationCard } from "../DeviceInformationCard"
import { LogsCard } from "../LogsCard"
import { MainAppBar } from "../MainAppBar"
import { PingCommandCard } from "../PingCommandCard"

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

      {!isConnected && (
        <Alert severity={"error"}>
          <AlertTitle>Not connected!</AlertTitle>
          <Button onClick={handleConnectClick}>Connect</Button>
        </Alert>
      )}

      <Grid container spacing={2} padding={2}>
        <Grid size={6}>
          <Stack spacing={2}>
            <PingCommandCard />
            <DeviceInformationCard />
          </Stack>
        </Grid>
        <Grid size={6}>
          <LogsCard />
        </Grid>
      </Grid>
    </Box>
  )
}