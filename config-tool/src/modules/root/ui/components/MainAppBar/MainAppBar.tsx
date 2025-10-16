import { AppBar, Button, Chip, Stack, Toolbar, Typography } from "@mui/material"
import { useDeviceContext } from "@src/modules/device/context"
import { useCallback } from "react"
import UsbOutlinedIcon from "@mui/icons-material/UsbOutlined"
import UsbOffOutlinedIcon from "@mui/icons-material/UsbOffOutlined"

export const MainAppBar = () => {

  const deviceContext = useDeviceContext()
  const { facade: deviceFacade, isConnected } = deviceContext

  const handleConnectClick = useCallback(() => {
    deviceFacade.connect().then()
  }, [deviceFacade])

  const handleDisconnectClick = useCallback(() => {
    deviceFacade.disconnect().then()
  }, [deviceFacade])

  return (
    <AppBar position={"static"}>
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
      </Toolbar>
    </AppBar>
  )
}