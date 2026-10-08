import { Card, CardContent, CardHeader, Grid, Paper, Stack, Typography } from "@mui/material"
import { useDeviceInformation } from "@src/modules/device/hooks/useDeviceInformation"
import { useConnectedDevices } from "@src/modules/device/hooks/useConnectedDevices"
import { type PushButtonStates, usePushButtonStates } from "@src/modules/device/hooks/usePushButtonStates"

import classes from "./DevicePreviewCard.module.css"


export const DevicePreviewCard = () => {
  const [deviceInformation] = useDeviceInformation()
  const connectedDevicesQuery = useConnectedDevices()

  const deviceType = deviceInformation?.deviceType ?? null
  const deviceId = deviceInformation?.deviceId ?? ""

  const subDevices = (connectedDevicesQuery.data ?? []).filter(it => it.parentDeviceId === deviceId)

  return (
    <Card>
      <CardHeader
        title={"Device Preview"}
        subheader={`Type ${deviceType}`}
      />
      <CardContent>
        <Stack spacing={2}>
          <PreviewCardContent deviceId={deviceId} deviceType={deviceType} />
          {
            subDevices.map(it => (
              <Stack key={it.deviceId} spacing={1}>
                <Typography variant={"subtitle2"} align={"center"}>
                  Sub-device <code>{it.deviceId}</code> (type <code>{it.deviceType}</code>)
                </Typography>
                <PreviewCardContent deviceId={it.deviceId} deviceType={it.deviceType} />
              </Stack>
            ))
          }
        </Stack>
      </CardContent>
    </Card>
  )
}

const PreviewCardContent = (props: {
  deviceId: string
  deviceType: string | null
}) => {
  const { deviceId, deviceType } = props

  switch (deviceType) {
    case "a":
      return <TypeAPreviewCardContent deviceId={deviceId} />
    case "smc1":
      return <TypeSmc1PreviewCardContent deviceId={deviceId} />
    case "smc2":
      return <TypeSmc2PreviewCardContent deviceId={deviceId} />
    default:
      return <NullPreviewCardContent />
  }
}

const NullPreviewCardContent = () => {
  return (
    <Typography><em>Unknown device type, no preview available.</em></Typography>
  )
}

const TypeAPreviewCardContent = (props: {
  deviceId: string,
}) => {
  const { deviceId } = props

  const pushButtonStates = usePushButtonStates(deviceId)

  return (
    <Paper elevation={5} className={classes.TypeAPreviewContainer}>
      <SwitchStateIndicatorCluster firstSwitchNumber={1} pushButtonStates={pushButtonStates} />
      <SwitchStateIndicatorCluster firstSwitchNumber={5} pushButtonStates={pushButtonStates} />
      <SwitchStateIndicatorCluster firstSwitchNumber={9} pushButtonStates={pushButtonStates} />
    </Paper>
  )
}

const TypeSmc1PreviewCardContent = (props: {
  deviceId: string,
}) => {
  const { deviceId } = props

  const pushButtonStates = usePushButtonStates(deviceId)

  return (
    <Paper elevation={5} className={classes.SubDevicePreviewContainer}>
      <SwitchStateIndicatorCluster firstSwitchNumber={1} pushButtonStates={pushButtonStates} />
    </Paper>
  )
}

const TypeSmc2PreviewCardContent = (props: {
  deviceId: string,
}) => {
  const { deviceId } = props

  const pushButtonStates = usePushButtonStates(deviceId)

  // 1u switches, so three of them span the same width as two of the 1.5u switches of the other clusters
  return (
    <Paper elevation={5} className={classes.SubDevicePreviewContainer}>
      <SwitchStateIndicatorCluster firstSwitchNumber={1} rows={2} columns={3} pushButtonStates={pushButtonStates} />
    </Paper>
  )
}

/**
 * A cluster of switches (2x2 by default), numbered from firstSwitchNumber left to right, top to bottom.
 */
const SwitchStateIndicatorCluster = (props: {
  firstSwitchNumber: number,
  rows?: number,
  columns?: number,
  pushButtonStates: PushButtonStates,
}) => {
  const { firstSwitchNumber, rows = 2, columns = 2, pushButtonStates } = props

  return (
    <Grid container spacing={1} className={classes.SwitchStateIndicatorCluster}>
      {
        Array.from({ length: rows * columns }, (_, offset) => {
          const switchNumber = firstSwitchNumber + offset
          return (
            <Grid key={switchNumber} size={12 / columns}>
              <SwitchStateIndicator switchNumber={switchNumber} isPressed={pushButtonStates[switchNumber] ?? false} />
            </Grid>
          )
        })
      }
    </Grid>
  )
}

const SwitchStateIndicator = (props: {
  switchNumber: number,
  isPressed: boolean
}) => {
  const { switchNumber, isPressed } = props

  return (
    <div
      className={classes.SwitchStateIndicator}
      data-isPressed={isPressed}
      children={<span>{switchNumber}</span>}
    />
  )
}
