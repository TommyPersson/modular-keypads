import { Grid, Stack } from "@mui/material"
import { ConnectedDevicesCard, MainDeviceCard } from "@src/modules/overview/ui/components"


export const OverviewPage = () => {
  return (
    <Grid container spacing={2} padding={2}>
      <Grid size={4}>
        <Stack spacing={2}>
          <MainDeviceCard />
        </Stack>
      </Grid>
      <Grid size={4}>
        <Stack spacing={2}>
          <ConnectedDevicesCard />
        </Stack>
      </Grid>
    </Grid>
  )
}