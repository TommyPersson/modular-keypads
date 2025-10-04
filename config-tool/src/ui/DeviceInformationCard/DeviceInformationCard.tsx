import { Button, Card, CardContent, Typography } from "@mui/material"
import { useQuery } from "@tanstack/react-query"
import { useDeviceContext } from "../../context/DeviceContext"

export const DeviceInformationCard = () => {
  const { facade: deviceFacade, isConnected } = useDeviceContext()

  const deviceIdQuery = useQuery({
    queryKey: ["device-information", "device-id"],
    queryFn: async (): Promise<DeviceInformation> => {
      return {
        deviceId: await deviceFacade.getDeviceId(),
        deviceFirmwareVersion: await deviceFacade.getDeviceFirmwareVersion(),
        deviceType: await deviceFacade.getDeviceType(),
        deviceAddress: await deviceFacade.getDeviceAddress(),
      };
    },
    enabled: isConnected,
  })

  return (
    <Card>
      <CardContent>
        <Typography variant="h5" component="div">Device Information</Typography>
        <Button onClick={() => deviceIdQuery.refetch()}>Refresh!</Button>
        <Typography>ID: <strong><code>{deviceIdQuery.data?.deviceId ?? "N/A"}</code></strong></Typography>
        <Typography>Firmware Version: <strong><code>{deviceIdQuery.data?.deviceFirmwareVersion ?? "N/A"}</code></strong></Typography>
        <Typography>Type: <strong><code>{deviceIdQuery.data?.deviceType ?? "N/A"}</code></strong></Typography>
        <Typography>Address: <strong><code>{deviceIdQuery.data?.deviceAddress ?? "N/A"}</code></strong></Typography>
      </CardContent>
    </Card>
  )
}

type DeviceInformation = {
  deviceId: string
  deviceFirmwareVersion: string
  deviceType: string
  deviceAddress: number
}