import { Grid, Stack } from "@mui/material"
import { Page } from "@src/modules/common/components"
import { ConnectedDevicesCard, DevicePreviewList } from "@src/modules/overview/ui/components"


export const OverviewPage = () => {
  return (
    <Page fillHeight>
      {/* Its single row is stretched to the page height, and the cells with it */}
      <Grid container spacing={2} flexGrow={1}>
        <Grid size={4}>
          <Stack spacing={2}>
            <ConnectedDevicesCard />
          </Stack>
        </Grid>
        <Grid size={8} minHeight={0}>
          <DevicePreviewList />
        </Grid>
      </Grid>
    </Page>
  )
}
