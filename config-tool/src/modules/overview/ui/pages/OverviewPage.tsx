import { Box, Divider, Paper, Stack } from "@mui/material"
import { Page } from "@src/modules/common/components"
import { ConfigurationTransferButtons } from "@src/modules/config-transfer/ui"
import { useDeviceContext } from "@src/modules/device/context"
import { DeviceConnectionControl, TestModeSwitch } from "@src/modules/device/ui"
import { DevicePreviewList } from "@src/modules/overview/ui/components"


export const OverviewPage = () => {
  const { isConnected } = useDeviceContext()

  return (
    <Page fillHeight noPadding>
      <Box sx={{ position: "relative", flexGrow: 1, minHeight: 0 }}>
        <DevicePreviewList />
        <Paper
          elevation={6}
          sx={{ position: "absolute", bottom: 16, left: "50%", transform: "translateX(-50%)", width: "max-content", borderRadius: 2, p: 0.5 }}
        >
          <Stack direction={"row"} alignItems={"center"} gap={0.5}>
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
      </Box>
    </Page>
  )
}
