import { Card, CardContent, CardHeader, Grid, Paper, Typography } from "@mui/material"
import { useDeviceFacade } from "@src/modules/device/context"
import type { DeviceFacade, NotificationMessage } from "@src/modules/device/facade"
import { useDeviceInformation } from "@src/modules/device/hooks/useDeviceInformation"
import { useEffect, useReducer } from "react"

import classes from "./DevicePreviewCard.module.css"


export const DevicePreviewCard = () => {
  const deviceFacade = useDeviceFacade()
  const [deviceInformation] = useDeviceInformation()

  const deviceType = deviceInformation?.deviceType ?? null

  return (
    <Card>
      <CardHeader
        title={"Device Preview"}
        subheader={`Type ${deviceType}`}
      />
      <CardContent>
        <PreviewCardContent deviceFacade={deviceFacade} deviceType={deviceType} />
      </CardContent>
    </Card>
  )
}

const PreviewCardContent = (props: {
  deviceFacade: DeviceFacade,
  deviceType: string | null
}) => {
  const { deviceFacade, deviceType } = props

  switch (deviceType) {
    case "a":
      return <TypeAPreviewCardContent deviceFacade={deviceFacade} />
    default:
      return <NullPreviewCardContent />
  }
}

const NullPreviewCardContent = () => {
  return (
    <Typography><em>Unknown device type, no preview available.</em></Typography>
  )
}

type SwitchStates = { [index: number]: boolean }

const TypeAPreviewCardContent = (props: {
  deviceFacade: DeviceFacade
}) => {
  const { deviceFacade } = props

  const [switchState, handleNotificationMessage] = useReducer<SwitchStates, [NotificationMessage]>((state, message) => {
    if (message.type !== "switch.pressed" && message.type !== "switch.released") {
      return state
    }

    const switchNumber = parseInt(message.args[0])
    return { ...state, [switchNumber]: message.type === "switch.pressed" }
  }, {})

  useEffect(() => {
    const subscription = deviceFacade.notifications$.subscribe(handleNotificationMessage)
    return () => subscription.unsubscribe()
    // eslint-disable-next-line
  }, [handleNotificationMessage])

  return (
    <Paper elevation={5} className={classes.TypeAPreviewContainer} >
      <Grid container spacing={1} className={classes.SwitchStateIndicatorCluster}>
        <Grid size={6}>
          <SwitchStateIndicator switchNumber={1} isPressed={switchState[1] ?? false} />
        </Grid>
        <Grid size={6}>
          <SwitchStateIndicator switchNumber={2} isPressed={switchState[2] ?? false} />
        </Grid>
        <Grid size={6}>
          <SwitchStateIndicator switchNumber={3} isPressed={switchState[3] ?? false} />
        </Grid>
        <Grid size={6}>
          <SwitchStateIndicator switchNumber={4} isPressed={switchState[4] ?? false} />
        </Grid>
      </Grid>
      <Grid container spacing={1} className={classes.SwitchStateIndicatorCluster}>
        <Grid size={6}>
          <SwitchStateIndicator switchNumber={5} isPressed={switchState[5] ?? false} />
        </Grid>
        <Grid size={6}>
          <SwitchStateIndicator switchNumber={6} isPressed={switchState[6] ?? false} />
        </Grid>
        <Grid size={6}>
          <SwitchStateIndicator switchNumber={7} isPressed={switchState[7] ?? false} />
        </Grid>
        <Grid size={6}>
          <SwitchStateIndicator switchNumber={8} isPressed={switchState[8] ?? false} />
        </Grid>
      </Grid>

      <Grid container spacing={1} className={classes.SwitchStateIndicatorCluster}>
        <Grid size={6}>
          <SwitchStateIndicator switchNumber={9} isPressed={switchState[9] ?? false} />
        </Grid>
        <Grid size={6}>
          <SwitchStateIndicator switchNumber={10} isPressed={switchState[10] ?? false} />
        </Grid>
        <Grid size={6}>
          <SwitchStateIndicator switchNumber={11} isPressed={switchState[11] ?? false} />
        </Grid>
        <Grid size={6}>
          <SwitchStateIndicator switchNumber={12} isPressed={switchState[12] ?? false} />
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