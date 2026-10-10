import ChevronLeftIcon from "@mui/icons-material/ChevronLeft"
import ChevronRightIcon from "@mui/icons-material/ChevronRight"
import { Box, IconButton, Stack, Tooltip } from "@mui/material"
import { TopLevelDevicePreviewCard } from "@src/modules/device-debugger/ui/components/DevicePreviewCard"
import { useConnectedDevices } from "@src/modules/device/hooks/useConnectedDevices"
import { useDeviceOrder } from "@src/modules/overview/hooks/useDeviceOrder"
import { useMemo } from "react"

/**
 * Previews all connected top-level devices side by side, in the order arranged by the user. Fills its parent, with
 * the cards centered in it, scrolling horizontally when they don't fit.
 */
export const DevicePreviewList = () => {
  const connectedDevicesQuery = useConnectedDevices()

  const topLevelDevices = useMemo(
    () => (connectedDevicesQuery.data ?? []).filter(it => it.parentDeviceId === null),
    [connectedDevicesQuery.data],
  )
  const { orderedDevices, moveDevice } = useDeviceOrder(topLevelDevices)

  return (
    <Box sx={{ width: "100%", height: "100%", overflow: "auto", display: "flex" }}>
      {/*
        Centered by auto margins rather than by justifying the content, as the margins collapse when the cards
        don't fit, so that all of them can still be scrolled to. Padded so the scroll area doesn't clip the shadows.
      */}
      <Stack direction={"row"} spacing={2} sx={{ m: "auto", p: 0.5, alignItems: "flex-start" }}>
        {
          orderedDevices.map((it, index) => (
            <TopLevelDevicePreviewCard
              key={it.deviceId}
              device={it}
              action={
                <>
                  <Tooltip title={"Move left"}>
                    <span>
                      <IconButton size={"small"} disabled={index === 0} onClick={() => moveDevice(it.deviceId, -1)}>
                        <ChevronLeftIcon />
                      </IconButton>
                    </span>
                  </Tooltip>
                  <Tooltip title={"Move right"}>
                    <span>
                      <IconButton
                        size={"small"}
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
    </Box>
  )
}
