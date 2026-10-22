import { Box, Fade } from "@mui/material"
import { Page } from "@src/modules/common/components"
import { useDeviceContext } from "@src/modules/device/context"
import { KeyBindingSelectionProvider, KeyBindingsPane } from "@src/modules/key-bindings/ui/components/KeyBindingsPane"
import { DevicePreviewList } from "@src/modules/overview/ui/components"
import { useToolPaneContext } from "@src/modules/root/ui/components/ToolPane/ToolPaneContext"
import {
  KEY_BINDINGS_PANE_HEIGHT,
  KEY_BINDINGS_PANE_MAX_WIDTH,
  overlayInsets
} from "@src/modules/root/ui/overlayLayout"


export const OverviewPage = () => (
  <KeyBindingSelectionProvider>
    <OverviewPageContent />
  </KeyBindingSelectionProvider>
)

/**
 * The devices, with the key bindings pane right below them while a device is connected.
 */
const OverviewPageContent = () => {
  const { selectedToolId } = useToolPaneContext()
  const { isConnected } = useDeviceContext()

  return (
    <Page fillHeight noPadding>
      <DevicePreviewList
        overlayInsets={overlayInsets(selectedToolId !== null)}
        footer={
          <Fade in={isConnected} unmountOnExit>
            <Box sx={{ height: KEY_BINDINGS_PANE_HEIGHT }}>
              <KeyBindingsPane />
            </Box>
          </Fade>
        }
        footerMaxWidth={KEY_BINDINGS_PANE_MAX_WIDTH}
      />
    </Page>
  )
}
