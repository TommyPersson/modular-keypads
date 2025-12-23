import EditOutlinedIcon from "@mui/icons-material/EditOutlined"

import {
  Divider, IconButton,
  type SelectChangeEvent,
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableRow,
  Typography
} from "@mui/material"
import { Card, CardContent, CardHeader, FormControl, InputLabel, ListItemText, MenuItem, Select } from "@mui/material"
import { useDeviceFacade } from "@src/modules/device/context"
import { usePushButtonStates } from "@src/modules/device/hooks/usePushButtonStates"
import { ListConnectedDevicesQuery } from "@src/modules/device/queries/ListConnectedDevicesQuery"
import { ListDeviceCapabilitiesQuery } from "@src/modules/device/queries/ListDeviceCapabilitiesQuery"
import { useQuery } from "@tanstack/react-query"
import { useCallback, useState } from "react"

import classes from "./EditKeyBindingsCard.module.css"

export const EditKeyBindingsCard = () => {
  const deviceFacade = useDeviceFacade()

  const devicesQuery = useQuery(ListConnectedDevicesQuery(deviceFacade))
  const devices = devicesQuery.data ?? []

  const [selectedDeviceId, setSelectedDeviceId] = useState<string | null>(devices?.[0]?.deviceId ?? null)
  const handleSelectedDeviceIdChanged = useCallback((e: SelectChangeEvent<string | null>) => {
    setSelectedDeviceId(e.target.value)
  }, [setSelectedDeviceId])

  //const selectedDevice = devices.find(it => it.deviceId === selectedDeviceId) ?? null

  const deviceCapabilitiesQuery = useQuery(ListDeviceCapabilitiesQuery(selectedDeviceId ?? "", deviceFacade))
  const deviceCapabilities = deviceCapabilitiesQuery.data ?? []

  const pushButtons = deviceCapabilities.filter(it => it.type === "PushButton")
  const pushButtonStates = usePushButtonStates(selectedDeviceId ?? "")

  return (
    <Card>
      <CardHeader title={"Key Bindings"} />
      <CardContent>
        <FormControl fullWidth>
          <InputLabel>Device</InputLabel>
          <Select
            label={"Device"}
            value={selectedDeviceId}
            onChange={handleSelectedDeviceIdChanged}
            className={classes.DeviceSelect}
          >
            {devices.map(device => (
              <MenuItem value={device.deviceId}>
                <ListItemText>
                  {device.deviceName}
                </ListItemText>
                <code>{device.deviceId}</code>
              </MenuItem>
            ))}
          </Select>
        </FormControl>
      </CardContent>
      {pushButtons.length > 0 && (
        <>
          <CardContent>
            <Typography variant={"subtitle2"}>
              Push Buttons
            </Typography>
            <Typography variant={"body1"}>
              You can press buttons on your device to highlight them in the list.
            </Typography>
          </CardContent>
          <CardContent style={{ paddingLeft: 0, paddingRight: 0 }}>
            <Table size={"small"}>
              <TableHead>
                <TableRow>
                  <TableCell style={{ width: 0, whiteSpace: "nowrap" }}>Button #</TableCell>
                  <TableCell>Current Binding</TableCell>
                  <TableCell style={{ width: 0 }}></TableCell>
                </TableRow>
              </TableHead>
              <TableBody>
                {pushButtons.map(it => {
                  const isPressed = pushButtonStates[it.number] ?? false
                  const backgroundColor = isPressed ? "info.light" : undefined
                  const color = isPressed ? "info.contrastText" : undefined

                  return (
                    <TableRow hover key={it.number} sx={{ backgroundColor, color }}>
                      <TableCell sx={{ color }}>
                        <strong>{it.number}</strong>
                      </TableCell>
                      <TableCell sx={{ color }}>
                        <em>None</em>
                      </TableCell>
                      <TableCell sx={{ color }}>
                        <IconButton><EditOutlinedIcon sx={{ color }} /></IconButton>
                      </TableCell>
                    </TableRow>
                  )
                })}
              </TableBody>
            </Table>
          </CardContent>
        </>
      )}
    </Card>
  )
}