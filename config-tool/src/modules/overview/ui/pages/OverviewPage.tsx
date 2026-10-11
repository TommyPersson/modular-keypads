import { Page } from "@src/modules/common/components"
import { DevicePreviewList } from "@src/modules/overview/ui/components"
import { useToolPaneContext } from "@src/modules/root/ui/components/ToolPane/ToolPaneContext"
import { overlayInsets } from "@src/modules/root/ui/overlayLayout"


export const OverviewPage = () => {
  const { selectedToolId } = useToolPaneContext()

  return (
    <Page fillHeight noPadding>
      <DevicePreviewList overlayInsets={overlayInsets(selectedToolId !== null)} />
    </Page>
  )
}
