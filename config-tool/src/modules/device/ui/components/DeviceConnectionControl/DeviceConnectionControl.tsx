import UsbOffOutlinedIcon from "@mui/icons-material/UsbOffOutlined"
import UsbOutlinedIcon from "@mui/icons-material/UsbOutlined"
import { Button, Chip, Stack } from "@mui/material"
import { useDeviceContext } from "@src/modules/device/context"
import { useCallback } from "react"

export const DeviceConnectionControl = () => {
  const { facade: deviceFacade, isConnected } = useDeviceContext()

  const handleConnectClick = useCallback(() => {
    deviceFacade.connect().then()
  }, [deviceFacade])

  const handleDisconnectClick = useCallback(() => {
    deviceFacade.disconnect().then()
  }, [deviceFacade])

  return (
    <Stack direction={"row"} alignItems={"center"} gap={1} sx={{ pl: 0.5 }}>
      {isConnected ? (
        <>
          <Chip label={"Connected"} variant={"filled"} color={"success"} icon={<UsbOutlinedIcon />} />
          <Button onClick={handleDisconnectClick}>Disconnect</Button>
        </>
      ) : (
        <>
          <Chip label={"Disconnected"} variant={"filled"} color={"error"} icon={<UsbOffOutlinedIcon />} />
          <Button variant={"contained"} onClick={handleConnectClick}>Connect</Button>
        </>
      )}
    </Stack>
  )
}
