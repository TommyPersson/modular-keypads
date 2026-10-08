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

/**
 * A 2x2 cluster of switches, numbered from firstSwitchNumber left to right, top to bottom.
 */
const SwitchStateIndicatorCluster = (props: {
  firstSwitchNumber: number,
  pushButtonStates: PushButtonStates,
}) => {
  const { firstSwitchNumber, pushButtonStates } = props

  return (
    <Grid container spacing={1} className={classes.SwitchStateIndicatorCluster}>
      {
        [0, 1, 2, 3].map(offset => {
          const switchNumber = firstSwitchNumber + offset
          return (
            <Grid key={switchNumber} size={6}>
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
