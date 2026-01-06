import { Grid, Stack } from "@mui/material"
import { Page } from "@src/modules/common/components"
import { ConnectedDevicesCard } from "@src/modules/overview/ui/components"


export const OverviewPage = () => {
  return (
    <Page>
      <Grid container spacing={2}>
        <Grid size={4}>
          <Stack spacing={2}>
            <ConnectedDevicesCard />
          </Stack>
        </Grid>
      </Grid>
    </Page>
  )
}