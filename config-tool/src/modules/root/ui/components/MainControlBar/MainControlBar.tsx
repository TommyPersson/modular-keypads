import { Divider, Paper, Stack } from "@mui/material"
import { ConfigurationTransferButtons } from "@src/modules/config-transfer/ui"
import { useDeviceContext } from "@src/modules/device/context"
import { DeviceConnectionControl, TestModeSwitch } from "@src/modules/device/ui"
import { MAIN_CONTROL_BAR_BOTTOM, MAIN_CONTROL_BAR_HEIGHT } from "@src/modules/root/ui/overlayLayout"

/**
 * Floats at the bottom center of the window, above everything else. Fixed, so that the space the bar takes can be
 * kept clear below the tool panes. High enough to leave room for the page's horizontal scrollbar below it.
 */
export const MainControlBar = () => {
  const { isConnected } = useDeviceContext()

  return (
    <Paper
      elevation={6}
      sx={{
        position: "fixed",
        bottom: MAIN_CONTROL_BAR_BOTTOM,
        left: "50%",
        transform: "translateX(-50%)",
        zIndex: theme => theme.zIndex.appBar,
        width: "max-content",
        height: MAIN_CONTROL_BAR_HEIGHT,
        display: "flex",
        alignItems: "center",
        borderRadius: 2,
        px: 0.5,
      }}
    >
      <Stack direction={"row"} alignItems={"center"} gap={0.5} alignSelf={"stretch"} py={0.5}>
        <DeviceConnectionControl />
        {isConnected && (
          <>
            <Divider orientation={"vertical"} flexItem />
            <TestModeSwitch />
            <Divider orientation={"vertical"} flexItem />
            <ConfigurationTransferButtons />
          </>
        )}
      </Stack>
    </Paper>
  )
}
