import UsbOffOutlinedIcon from "@mui/icons-material/UsbOffOutlined"
import UsbOutlinedIcon from "@mui/icons-material/UsbOutlined"
import { AppBar, Button, Chip, FormControlLabel, Stack, Switch, Toolbar, Tooltip, Typography } from "@mui/material"
import { DeviceLogsDropDown, DeviceMetricsDropDown } from "@src/modules/device-debugger/ui"
import { useDeviceContext } from "@src/modules/device/context"
import { GetTestModeQuery } from "@src/modules/device/queries/GetTestModeQuery"
import { queryClient } from "@src/utils/queryClient"
import { useMutation, useQuery } from "@tanstack/react-query"
import { useCallback, useEffect } from "react"

export const MainAppBar = () => {

  const deviceContext = useDeviceContext()
  const { facade: deviceFacade, isConnected } = deviceContext

  const handleConnectClick = useCallback(() => {
    deviceFacade.connect().then()
  }, [deviceFacade])

  const handleDisconnectClick = useCallback(() => {
    deviceFacade.disconnect().then()
  }, [deviceFacade])

  const testModeQuery = useQuery(GetTestModeQuery(deviceFacade))
  const testMode = testModeQuery.data ?? false

  const toggleTestMode = useMutation({
      mutationFn: async (args: { enabled: boolean }) => {
        await deviceFacade.setTestMode(args.enabled)
      },
      onSuccess: async () => {
        await queryClient.invalidateQueries({ queryKey: ["test-mode-state"] })
      }
    }
  )

  const handleTestModeToggled = useCallback(async () => {
    await toggleTestMode.mutateAsync({ enabled: !testMode })
  }, [testMode, toggleTestMode])

  useEffect(() => {
    queryClient.resetQueries().then() // TODO give all device queries a prefix
  }, [isConnected])

  return (
    <AppBar position={"fixed"} sx={{ zIndex: (theme) => theme.zIndex.drawer + 1 }}>
      <Toolbar>
        <Stack direction={"row"} spacing={2}>
          <Typography variant="h6" component="div">
            Config Tool
          </Typography>

          {isConnected && (
            <Chip label={"Connected"} variant={"filled"} color={"success"} icon={<UsbOutlinedIcon />} />
          )}
          {!isConnected && (
            <Chip label={"Disconnected"} variant={"filled"} color={"error"} icon={<UsbOffOutlinedIcon />} />
          )}
        </Stack>


        <div style={{ flexGrow: 1 }} />

        <Stack direction={"row"} spacing={2} alignItems={"center"}>
          <DeviceMetricsDropDown />
          <DeviceLogsDropDown />

          <Chip
            color={"default"}
            variant={"outlined"}
            style={{ color: "inherit" }}
            label={
              <Tooltip title={"No keybindings will be executed while test mode is active"}>
                <FormControlLabel
                  checked={testMode}
                  label={`Test Mode: ${testMode ? "On" : "Off"}`}
                  control={
                    <Switch
                      value={testMode}
                      color={testMode ? "default" : "error"}
                      onClick={handleTestModeToggled}
                    />
                  }
                />
              </Tooltip>
            } />

          {isConnected && (
            <>
              <Button color={"inherit"} onClick={handleDisconnectClick}>
                Disconnect
              </Button>
            </>
          )}
          {!isConnected && (
            <Button color={"inherit"} onClick={handleConnectClick}>
              Connect
            </Button>
          )}
        </Stack>
      </Toolbar>
    </AppBar>
  )
}