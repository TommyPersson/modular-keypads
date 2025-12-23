import { Grid } from "@mui/material"
import { Page } from "@src/modules/common/components"
import { EditKeyBindingsCard } from "@src/modules/key-bindings/ui/components"


export const KeyBindingsPage = () => {
  return (
    <Page>
      <Grid container spacing={2}>
        <Grid size={4}>
          <EditKeyBindingsCard />
        </Grid>
      </Grid>
    </Page>
  )
}