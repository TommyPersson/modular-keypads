import DeleteSweepOutlinedIcon from "@mui/icons-material/DeleteSweepOutlined"
import SwapVertOutlinedIcon from "@mui/icons-material/SwapVertOutlined"
import { Button, CardContent, CardHeader, Stack } from "@mui/material"
import { CommandButton } from "@src/modules/common/components"
import { PingDeviceCommand } from "@src/modules/device-debugger/commands"
import { useDeviceContext } from "@src/modules/device/context"
import type { DeviceMessage } from "@src/modules/device/models"
import { useCallback, useEffect, useRef, useState } from "react"

export const DeviceTrafficToolPaneContent = () => {
  const state = useDeviceTrafficToolPaneState()

  return (
    <Stack>
      <CardHeader
        title={
          <Stack spacing={1} direction={"row"} alignItems={"center"}>
            <SwapVertOutlinedIcon />
            <span>Device Serial Traffic</span>
          </Stack>
        }
        action={(
          <Stack direction={"row"} gap={2}>
            <CommandButton
              command={PingDeviceCommand}
              args={{}}
              disabled={!state.isConnected}
            />
            <Button
              startIcon={<DeleteSweepOutlinedIcon />}
              onClick={state.handleClearClick}
              children={"Clear"}
            />
          </Stack>
        )}
      />
      <CardContent>
        <pre style={{
          overflow: "auto",
          minHeight: 100,
          maxHeight: "75vh",
          margin: 0,
          background: "var(--app-log-background)",
          padding: 8,
          fontSize: 14,
        }} ref={state.scrollViewRef}>
          {state.traffic.length === 0 && <em>No serial traffic has been recorded, yet.</em>}
          {state.traffic.map(it => formatLogLine(it))}
        </pre>
      </CardContent>
    </Stack>
  )
}

function useDeviceTrafficToolPaneState() {
  const { facade: deviceFacade, isConnected } = useDeviceContext()

  const scrollViewRef = useRef<HTMLPreElement | null>(null)
  const [traffic, setTraffic] = useState<ReadonlyArray<DeviceMessage>>(deviceFacade.traffic)

  useEffect(() => {
    const subscription = deviceFacade.traffic$.subscribe(setTraffic)
    return () => subscription.unsubscribe()
  }, [deviceFacade])

  useEffect(() => {
    const scrollView = scrollViewRef.current
    if (scrollView) {
      scrollView.scrollTo({ top: scrollView.scrollHeight })
    }
  }, [traffic])

  const handleClearClick = useCallback(() => {
    deviceFacade.clearTraffic()
  }, [deviceFacade])

  return {
    traffic,
    isConnected,
    scrollViewRef,
    handleClearClick,
  }
}

function formatLogLine(message: DeviceMessage) {
  const direction = message.direction === "to-host" ? "<" : ">"
  return `[${message.timestamp.toFormat("HH:mm:ss.SSS")}] ${direction} ${message.message}\n`
}
