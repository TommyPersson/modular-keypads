import ChevronLeftIcon from "@mui/icons-material/ChevronLeft"
import ChevronRightIcon from "@mui/icons-material/ChevronRight"
import { Grid, IconButton, Stack, Tooltip } from "@mui/material"
import { Page } from "@src/modules/common/components"
import { TopLevelDevicePreviewCard } from "@src/modules/device-debugger/ui/components/DevicePreviewCard"
import { useConnectedDevices } from "@src/modules/device/hooks/useConnectedDevices"
import { useDeviceOrder } from "@src/modules/overview/hooks/useDeviceOrder"
import { ConnectedDevicesCard } from "@src/modules/overview/ui/components"
import { useMemo } from "react"


export const OverviewPage = () => {
  const connectedDevicesQuery = useConnectedDevices()

  const topLevelDevices = useMemo(
    () => (connectedDevicesQuery.data ?? []).filter(it => it.parentDeviceId === null),
    [connectedDevicesQuery.data],
  )
  const { orderedDevices, moveDevice } = useDeviceOrder(topLevelDevices)

  return (
    <Page>
      <Grid container spacing={2}>
        <Grid size={4}>
          <Stack spacing={2}>
            <ConnectedDevicesCard />
          </Stack>
        </Grid>
        <Grid size={8}>
          {/* Side by side, scrolling when they don't fit. Padded so the scroll area doesn't clip the shadows. */}
          <Stack direction={"row"} spacing={2} sx={{ overflowX: "auto", alignItems: "flex-start", p: 0.5, m: -0.5 }}>
            {
              orderedDevices.map((it, index) => (
                <TopLevelDevicePreviewCard
                  key={it.deviceId}
                  title={it.deviceName}
                  deviceId={it.deviceId}
                  deviceType={it.deviceType}
                  action={
                    <>
                      <Tooltip title={"Move left"}>
                        <span>
                          <IconButton disabled={index === 0} onClick={() => moveDevice(it.deviceId, -1)}>
                            <ChevronLeftIcon />
                          </IconButton>
                        </span>
                      </Tooltip>
                      <Tooltip title={"Move right"}>
                        <span>
                          <IconButton
                            disabled={index === orderedDevices.length - 1}
                            onClick={() => moveDevice(it.deviceId, 1)}
                          >
                            <ChevronRightIcon />
                          </IconButton>
                        </span>
                      </Tooltip>
                    </>
                  }
                />
              ))
            }
          </Stack>
        </Grid>
      </Grid>
    </Page>
  )
}
