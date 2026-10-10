import { Grid } from "@mui/material"
import { Page } from "@src/modules/common/components"
import { EditSettingsCard } from "@src/modules/settings/ui/components"

export const SettingsPage = () => {
  return (
    <Page>
      <Grid container spacing={2}>
        <Grid size={8}>
          <EditSettingsCard />
        </Grid>
      </Grid>
    </Page>
  )
}
