import { Card, CardContent, CardHeader, Grid, Paper, Stack, Typography } from "@mui/material"
import type { ReactNode } from "react"
import type { DeviceInformation } from "@src/modules/device/models"
import { useDeviceInformation } from "@src/modules/device/hooks/useDeviceInformation"
import { useConnectedDevices } from "@src/modules/device/hooks/useConnectedDevices"
import {
  isPushButtonPressed,
  PushButtonState,
  type PushButtonStates,
  usePushButtonStates
} from "@src/modules/device/hooks/usePushButtonStates"
import { useRotaryEncoderStates } from "@src/modules/device/hooks/useRotaryEncoderStates"

import classes from "./DevicePreviewCard.module.css"

// Type C devices have a fixed number of sub-device slots, and never report more sub-devices than that
const TYPE_C_SUB_DEVICE_SLOTS = 3

/**
 * Previews the device the tool is connected to, with its sub-devices.
 */
export const DevicePreviewCard = () => {
  const [deviceInformation] = useDeviceInformation()

  return (
    <TopLevelDevicePreviewCard
      title={"Device Preview"}
      deviceId={deviceInformation?.deviceId ?? ""}
      deviceType={deviceInformation?.deviceType ?? null}
    />
  )
}

/**
 * Previews a device that isn't a sub-device, with its sub-devices. The action is shown in the header.
 */
export const TopLevelDevicePreviewCard = (props: {
  title: ReactNode,
  action?: ReactNode,
  deviceId: string,
  deviceType: string | null,
}) => {
  const { title, action, deviceId, deviceType } = props

  return (
    // Not shrunk, as the previews have a fixed width
    <Card sx={{ flexShrink: 0 }}>
      <CardHeader
        title={title}
        subheader={`Type ${deviceType}`}
        action={action}
      />
      <CardContent>
        <PreviewCardContent deviceId={deviceId} deviceType={deviceType} />
      </CardContent>
    </Card>
  )
}

/**
 * The preview of a top-level device, which includes its sub-devices.
 */
const PreviewCardContent = (props: {
  deviceId: string
  deviceType: string | null
}) => {
  const { deviceId, deviceType } = props

  switch (deviceType) {
    case "a":
      return <TypeAPreviewCardContent deviceId={deviceId} />
    case "c":
      return <TypeCPreviewCardContent deviceId={deviceId} />
    default:
      return <UnknownTypePreviewText />
  }
}

const UnknownTypePreviewText = () => (
  <Typography><em>Unknown device type, no preview available.</em></Typography>
)

const TypeCPreviewCardContent = (props: {
  deviceId: string,
}) => {
  const { deviceId } = props

  const connectedDevicesQuery = useConnectedDevices()

  const subDevices = (connectedDevicesQuery.data ?? []).filter(it => it.parentDeviceId === deviceId)

  // Disconnected sub-devices aren't listed, so their slots are shown as placeholders, keeping the layout steady
  const subDeviceSlots = Array.from({ length: TYPE_C_SUB_DEVICE_SLOTS }, (_, index) => (
    subDevices.find(it => it.subDeviceSlot === index + 1) ?? null
  ))

  return (
    <Stack spacing={2}>
      {
        subDeviceSlots.map((it, index) => (
          it !== null
            ? <SubDevicePreview key={it.deviceId} subDevice={it} />
            : <DisconnectedSubDevicePreview key={`disconnected-${index}`} slot={index + 1} />
        ))
      }
    </Stack>
  )
}

/**
 * The caption, shown in the bottom-right corner, identifies the sub-device.
 */
const SubDevicePreview = (props: {
  subDevice: DeviceInformation,
}) => {
  const { subDevice } = props
  const { deviceId, deviceType } = subDevice

  const caption = <>Sub-device <code>{deviceId}</code> (type <code>{deviceType}</code>)</>

  switch (deviceType) {
    case "smc1":
      return <TypeSmc1PreviewCardContent deviceId={deviceId} caption={caption} />
    case "smc2":
      return <TypeSmc2PreviewCardContent deviceId={deviceId} caption={caption} />
    case "smc3":
      return <TypeSmc3PreviewCardContent deviceId={deviceId} caption={caption} />
    default:
      return (
        <SubDevicePreviewContainer caption={caption}>
          <UnknownTypePreviewText />
        </SubDevicePreviewContainer>
      )
  }
}

/**
 * Takes the same space as the sub-device previews, which are all equally high.
 */
const DisconnectedSubDevicePreview = (props: {
  slot: number,
}) => {
  const { slot } = props

  return (
    <SubDevicePreviewContainer elevation={0} caption={<>Slot {slot}</>}>
      <div className={classes.DisconnectedSubDevicePreview}>
        <Typography variant={"body2"} color={"text.secondary"}><em>Not connected</em></Typography>
      </div>
    </SubDevicePreviewContainer>
  )
}

/**
 * The caption is placed in the bottom padding, so it doesn't change the size of the preview.
 */
const SubDevicePreviewContainer = (props: {
  caption: ReactNode,
  elevation?: number,
  children: ReactNode,
}) => {
  const { caption, elevation = 5, children } = props

  return (
    <Paper elevation={elevation} className={classes.SubDevicePreviewContainer}>
      {children}
      <span className={classes.SubDevicePreviewCaption}>{caption}</span>
    </Paper>
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
  caption: ReactNode,
}) => {
  const { deviceId, caption } = props

  const pushButtonStates = usePushButtonStates(deviceId)

  return (
    <SubDevicePreviewContainer caption={caption}>
      <SwitchStateIndicatorCluster firstSwitchNumber={1} pushButtonStates={pushButtonStates} />
    </SubDevicePreviewContainer>
  )
}

const TypeSmc2PreviewCardContent = (props: {
  deviceId: string,
  caption: ReactNode,
}) => {
  const { deviceId, caption } = props

  const pushButtonStates = usePushButtonStates(deviceId)

  // 1u switches, so three of them span the same width as two of the 1.5u switches of the other clusters
  return (
    <SubDevicePreviewContainer caption={caption}>
      <SwitchStateIndicatorCluster firstSwitchNumber={1} rows={2} columns={3} pushButtonStates={pushButtonStates} />
    </SubDevicePreviewContainer>
  )
}

const TypeSmc3PreviewCardContent = (props: {
  deviceId: string,
  caption: ReactNode,
}) => {
  const { deviceId, caption } = props

  const pushButtonStates = usePushButtonStates(deviceId)
  const rotaryEncoderStates = useRotaryEncoderStates(deviceId, 300)

  return (
    <SubDevicePreviewContainer caption={caption}>
      <RotaryEncoderStateIndicator
        encoderNumber={1}
        isPressed={isPushButtonPressed(pushButtonStates[1])}
        isLongPressed={pushButtonStates[1] === PushButtonState.LongPressed}
        isRotatingClockwise={rotaryEncoderStates[1]?.clockwise ?? false}
        isRotatingCounterClockwise={rotaryEncoderStates[1]?.counterClockwise ?? false}
      />
    </SubDevicePreviewContainer>
  )
}

/**
 * A rotary encoder with a push button, drawn as the button in the center wrapped by two half rings, which light
 * up when the encoder is rotated in their direction: counter-clockwise on the left, clockwise on the right.
 *
 * The data attributes are lowercase, as Chrome doesn't restyle SVG elements when mixed-case attributes change.
 */
const RotaryEncoderStateIndicator = (props: {
  encoderNumber: number,
  isPressed: boolean,
  isLongPressed: boolean,
  isRotatingClockwise: boolean,
  isRotatingCounterClockwise: boolean,
}) => {
  const { encoderNumber, isPressed, isLongPressed, isRotatingClockwise, isRotatingCounterClockwise } = props

  return (
    <svg viewBox={"0 0 120 120"} className={classes.RotaryEncoderStateIndicator}>
      <path
        d={halfRingPath(60, 60, 36, 56, "left")}
        className={classes.RotaryEncoderStateIndicatorRing}
        data-active={isRotatingCounterClockwise}
      />
      <path
        d={halfRingPath(60, 60, 36, 56, "right")}
        className={classes.RotaryEncoderStateIndicatorRing}
        data-active={isRotatingClockwise}
      />
      <circle
        cx={60}
        cy={60}
        r={30}
        className={classes.RotaryEncoderStateIndicatorButton}
        data-pressed={isPressed}
        data-long-pressed={isLongPressed}
      />
      <text x={60} y={60} className={classes.RotaryEncoderStateIndicatorLabel}>{encoderNumber}</text>
    </svg>
  )
}

/**
 * SVG path of the left or right half of a ring. The halves are separated by a vertical gap at the top and bottom.
 */
function halfRingPath(cx: number, cy: number, innerRadius: number, outerRadius: number, side: "left" | "right") {
  const halfGap = 3
  const x = cx + (side === "right" ? halfGap : -halfGap)
  const sweep = side === "right" ? 1 : 0
  const outerY = Math.sqrt(outerRadius * outerRadius - halfGap * halfGap)
  const innerY = Math.sqrt(innerRadius * innerRadius - halfGap * halfGap)

  return [
    `M ${x} ${cy - outerY}`,
    `A ${outerRadius} ${outerRadius} 0 0 ${sweep} ${x} ${cy + outerY}`,
    `L ${x} ${cy + innerY}`,
    `A ${innerRadius} ${innerRadius} 0 0 ${1 - sweep} ${x} ${cy - innerY}`,
    "Z",
  ].join(" ")
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
              <SwitchStateIndicator switchNumber={switchNumber} state={pushButtonStates[switchNumber]} />
            </Grid>
          )
        })
      }
    </Grid>
  )
}

const SwitchStateIndicator = (props: {
  switchNumber: number,
  state: PushButtonState | undefined,
}) => {
  const { switchNumber, state } = props

  return (
    <div
      className={classes.SwitchStateIndicator}
      data-pressed={isPushButtonPressed(state)}
      data-long-pressed={state === PushButtonState.LongPressed}
      children={<span>{switchNumber}</span>}
    />
  )
}
