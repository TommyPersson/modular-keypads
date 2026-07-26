import { Grid } from "@mui/material"
import { Page } from "@src/modules/common/components"
import { EditKeyBindingsCard } from "@src/modules/key-bindings/ui/components"
import { EditMacrosCard } from "@src/modules/macros/ui"


export const KeyBindingsPage = () => {
  return (
    <Page>
      <Grid container spacing={2}>
        <Grid size={6}>
          <EditKeyBindingsCard />
        </Grid>
        <Grid size={6}>
          <EditMacrosCard />
        </Grid>
      </Grid>
    </Page>
  )
}