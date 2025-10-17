import { Grid, Stack } from "@mui/material"
import { DevicePreviewCard } from "@src/modules/device-debugger/ui/components/DevicePreviewCard"
import { DeviceInformationCard, DeviceRegistersCard, DeviceTrafficLogsCard, PingCommandCard } from "../components"

export const DeviceDebuggerPage = () => {
  return (
    <Grid container spacing={2} padding={2}>
      <Grid size={4}>
        <Stack spacing={2}>
          <DeviceInformationCard />
          <DeviceRegistersCard />
          <PingCommandCard />
        </Stack>
      </Grid>
      <Grid size={2}>
        <DevicePreviewCard />
      </Grid>
      <Grid size={6}>
        <DeviceTrafficLogsCard />
      </Grid>
    </Grid>
  )
}