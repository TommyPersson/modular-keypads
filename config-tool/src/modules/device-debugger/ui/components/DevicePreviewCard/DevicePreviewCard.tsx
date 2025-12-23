import { Card, CardContent, CardHeader, Grid, Paper, Typography } from "@mui/material"
import { useDeviceFacade } from "@src/modules/device/context"
import type { DeviceFacade, NotificationMessage } from "@src/modules/device/facade"
import { useDeviceInformation } from "@src/modules/device/hooks/useDeviceInformation"
import { usePushButtonStates } from "@src/modules/device/hooks/usePushButtonStates"
import { useEffect, useReducer } from "react"

import classes from "./DevicePreviewCard.module.css"


export const DevicePreviewCard = () => {
  const deviceFacade = useDeviceFacade()
  const [deviceInformation] = useDeviceInformation()

  const deviceType = deviceInformation?.deviceType ?? null
  const deviceId = deviceInformation?.deviceId ?? ""

  return (
    <Card>
      <CardHeader
        title={"Device Preview"}
        subheader={`Type ${deviceType}`}
      />
      <CardContent>
        <PreviewCardContent deviceId={deviceId} deviceType={deviceType} />
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
    <Paper elevation={5} className={classes.TypeAPreviewContainer} >
      <Grid container spacing={1} className={classes.SwitchStateIndicatorCluster}>
        <Grid size={6}>
          <SwitchStateIndicator switchNumber={1} isPressed={pushButtonStates[1] ?? false} />
        </Grid>
        <Grid size={6}>
          <SwitchStateIndicator switchNumber={2} isPressed={pushButtonStates[2] ?? false} />
        </Grid>
        <Grid size={6}>
          <SwitchStateIndicator switchNumber={3} isPressed={pushButtonStates[3] ?? false} />
        </Grid>
        <Grid size={6}>
          <SwitchStateIndicator switchNumber={4} isPressed={pushButtonStates[4] ?? false} />
        </Grid>
      </Grid>
      <Grid container spacing={1} className={classes.SwitchStateIndicatorCluster}>
        <Grid size={6}>
          <SwitchStateIndicator switchNumber={5} isPressed={pushButtonStates[5] ?? false} />
        </Grid>
        <Grid size={6}>
          <SwitchStateIndicator switchNumber={6} isPressed={pushButtonStates[6] ?? false} />
        </Grid>
        <Grid size={6}>
          <SwitchStateIndicator switchNumber={7} isPressed={pushButtonStates[7] ?? false} />
        </Grid>
        <Grid size={6}>
          <SwitchStateIndicator switchNumber={8} isPressed={pushButtonStates[8] ?? false} />
        </Grid>
      </Grid>

      <Grid container spacing={1} className={classes.SwitchStateIndicatorCluster}>
        <Grid size={6}>
          <SwitchStateIndicator switchNumber={9} isPressed={pushButtonStates[9] ?? false} />
        </Grid>
        <Grid size={6}>
          <SwitchStateIndicator switchNumber={10} isPressed={pushButtonStates[10] ?? false} />
        </Grid>
        <Grid size={6}>
          <SwitchStateIndicator switchNumber={11} isPressed={pushButtonStates[11] ?? false} />
        </Grid>
        <Grid size={6}>
          <SwitchStateIndicator switchNumber={12} isPressed={pushButtonStates[12] ?? false} />
        </Grid>
      </Grid>
    </Paper>
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
