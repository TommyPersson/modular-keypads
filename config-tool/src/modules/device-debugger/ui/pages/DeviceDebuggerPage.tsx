import { Grid, Stack } from "@mui/material"
import { Page } from "@src/modules/common/components"
import { ConnectedDevicesCard } from "@src/modules/device-debugger/ui/components/ConnectedDevicesCard"
import { DevicePreviewCard } from "@src/modules/device-debugger/ui/components/DevicePreviewCard"
import { DeviceInformationCard, DeviceRegistersCard, DeviceTrafficLogsCard, PingCommandCard } from "../components"

export const DeviceDebuggerPage = () => {
  return (
    <Page>
      <Grid container spacing={2}>
        <Grid size={4}>
          <Stack spacing={2}>
            <DeviceInformationCard />
            <DeviceRegistersCard />
            <ConnectedDevicesCard />
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
    </Page>
  )
}