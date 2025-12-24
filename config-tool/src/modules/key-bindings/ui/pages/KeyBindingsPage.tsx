import { Grid } from "@mui/material"
import { Page } from "@src/modules/common/components"
import { EditKeyBindingsCard, EditMacrosCard, KeyboardEventsCard } from "@src/modules/key-bindings/ui/components"


export const KeyBindingsPage = () => {
  return (
    <Page>
      <Grid container spacing={2}>
        <Grid size={4}>
          <EditKeyBindingsCard />
        </Grid>
        <Grid size={4}>
          <EditMacrosCard />
        </Grid>
        <Grid size={4}>
          <KeyboardEventsCard />
        </Grid>
      </Grid>
    </Page>
  )
}