import { Card, CardContent, CardHeader, Grid, Paper, Stack, Typography } from "@mui/material"
import type { ReactNode } from "react"
import type { DeviceInformation } from "@src/modules/device/models"
import { useDeviceInformation } from "@src/modules/device/hooks/useDeviceInformation"
import { useConnectedDevices } from "@src/modules/device/hooks/useConnectedDevices"
import { type PushButtonStates, usePushButtonStates } from "@src/modules/device/hooks/usePushButtonStates"
import { useRotaryEncoderStates } from "@src/modules/device/hooks/useRotaryEncoderStates"

import classes from "./DevicePreviewCard.module.css"

const SUB_DEVICE_SLOTS_BY_DEVICE_TYPE: { [deviceType: string]: number } = {
  c: 3,
}

export const DevicePreviewCard = () => {
  const [deviceInformation] = useDeviceInformation()
  const connectedDevicesQuery = useConnectedDevices()

  const deviceType = deviceInformation?.deviceType ?? null
  const deviceId = deviceInformation?.deviceId ?? ""

  const subDevices = (connectedDevicesQuery.data ?? []).filter(it => it.parentDeviceId === deviceId)

  // Disconnected sub-devices aren't listed, so their slots are shown as placeholders, keeping the layout steady
  const numSubDeviceSlots = SUB_DEVICE_SLOTS_BY_DEVICE_TYPE[deviceType ?? ""] ?? 0
  const subDeviceSlots = Array.from({ length: numSubDeviceSlots }, (_, index) => (
    subDevices.find(it => it.subDeviceSlot === index + 1) ?? null
  ))
  // Sub-devices outside the known slots are still shown, after them
  const otherSubDevices = subDevices.filter(it => !subDeviceSlots.includes(it))

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
            subDeviceSlots.map((it, index) => (
              it !== null
                ? <SubDevicePreview key={it.deviceId} subDevice={it} />
                : <DisconnectedSubDevicePreview key={`disconnected-${index}`} slot={index + 1} />
            ))
          }
          {
            otherSubDevices.map(it => <SubDevicePreview key={it.deviceId} subDevice={it} />)
          }
        </Stack>
      </CardContent>
    </Card>
  )
}

/**
 * The caption is shown in the bottom-right corner of sub-device previews, it isn't used for other devices.
 */
const PreviewCardContent = (props: {
  deviceId: string
  deviceType: string | null
  caption?: ReactNode
}) => {
  const { deviceId, deviceType, caption } = props

  switch (deviceType) {
    case "a":
      return <TypeAPreviewCardContent deviceId={deviceId} />
    case "smc1":
      return <TypeSmc1PreviewCardContent deviceId={deviceId} caption={caption} />
    case "smc2":
      return <TypeSmc2PreviewCardContent deviceId={deviceId} caption={caption} />
    case "smc3":
      return <TypeSmc3PreviewCardContent deviceId={deviceId} caption={caption} />
    default:
      return <NullPreviewCardContent caption={caption} />
  }
}

const NullPreviewCardContent = (props: {
  caption?: ReactNode,
}) => {
  const { caption } = props

  const text = <Typography><em>Unknown device type, no preview available.</em></Typography>

  if (caption === undefined) {
    return text
  }

  return (
    <SubDevicePreviewContainer caption={caption}>
      {text}
    </SubDevicePreviewContainer>
  )
}

const SubDevicePreview = (props: {
  subDevice: DeviceInformation,
}) => {
  const { subDevice } = props

  return (
    <PreviewCardContent
      deviceId={subDevice.deviceId}
      deviceType={subDevice.deviceType}
      caption={<>Sub-device <code>{subDevice.deviceId}</code> (type <code>{subDevice.deviceType}</code>)</>}
    />
  )
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
  caption?: ReactNode,
  elevation?: number,
  children: ReactNode,
}) => {
  const { caption, elevation = 5, children } = props

  return (
    <Paper elevation={elevation} className={classes.SubDevicePreviewContainer}>
      {children}
      {caption !== undefined && <span className={classes.SubDevicePreviewCaption}>{caption}</span>}
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
  caption?: ReactNode,
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
  caption?: ReactNode,
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
  caption?: ReactNode,
}) => {
  const { deviceId, caption } = props

  const pushButtonStates = usePushButtonStates(deviceId)
  const rotaryEncoderStates = useRotaryEncoderStates(deviceId, 300)

  return (
    <SubDevicePreviewContainer caption={caption}>
      <RotaryEncoderStateIndicator
        encoderNumber={1}
        isPressed={pushButtonStates[1] ?? false}
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
  isRotatingClockwise: boolean,
  isRotatingCounterClockwise: boolean,
}) => {
  const { encoderNumber, isPressed, isRotatingClockwise, isRotatingCounterClockwise } = props

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
