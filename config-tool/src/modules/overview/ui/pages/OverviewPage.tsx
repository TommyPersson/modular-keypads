import { Page } from "@src/modules/common/components"
import { DevicePreviewList } from "@src/modules/overview/ui/components"


export const OverviewPage = () => {
  return (
    <Page fillHeight noPadding>
      <DevicePreviewList />
    </Page>
  )
}
