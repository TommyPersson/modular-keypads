import { Grid, Stack } from "@mui/material"
import { DeviceInformationCard, DeviceRegistersCard, LogsCard, PingCommandCard } from "../components"

export const DeviceDebuggerPage = () => {
  return (
    <Grid container spacing={2} padding={2}>
      <Grid size={6}>
        <Stack spacing={2}>
          <PingCommandCard />
          <DeviceInformationCard />
          <DeviceRegistersCard />
        </Stack>
      </Grid>
      <Grid size={6}>
        <LogsCard />
      </Grid>
    </Grid>
  )
}