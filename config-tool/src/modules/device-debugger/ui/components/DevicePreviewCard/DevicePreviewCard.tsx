import { Box, Card, CardContent, Divider, Grid, Paper, Stack, Tooltip, Typography } from "@mui/material"
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
import {
  type PushButtonBinding,
  type PushButtonBindings,
  usePushButtonBindings
} from "@src/modules/key-bindings/hooks"
import type { MacroDefinition } from "@src/modules/macros/models"
import { MacroTypeVisualization } from "@src/modules/macros/ui"
import { DeviceInfoIcon, IdentifyDeviceButton, RenameDeviceButton } from "./DeviceActions"

import classes from "./DevicePreviewCard.module.css"

// Type C devices have a fixed number of sub-device slots, and never report more sub-devices than that
const TYPE_C_SUB_DEVICE_SLOTS = 3

// All top-level devices have the same physical dimensions, so their cards all have the same size
const TOP_LEVEL_CARD_WIDTH = 340
const TOP_LEVEL_CARD_HEIGHT = 670

/**
 * Previews the device the tool is connected to, with its sub-devices. Shows nothing until the device is known.
 */
export const DevicePreviewCard = () => {
  const [deviceInformation] = useDeviceInformation()
  const connectedDevicesQuery = useConnectedDevices()

  if (deviceInformation === null) {
    return null
  }

  // The listed device is refreshed when the device changes, e.g. when it's renamed, unlike the device information
  const device = connectedDevicesQuery.data?.find(it => it.deviceId === deviceInformation.deviceId)
    ?? deviceInformation

  return <TopLevelDevicePreviewCard device={device} />
}

/**
 * Previews a device that isn't a sub-device, with its sub-devices, and the actions for them.
 *
 * The device's actions are shown above its name, which gets the full width of the card, followed by the given action,
 * e.g. for placing the card, and the info icon in the top-right corner. The actions of the sub-devices follow their
 * captions.
 */
export const TopLevelDevicePreviewCard = (props: {
  device: DeviceInformation,
  action?: ReactNode,
}) => {
  const { device, action } = props
  const { deviceId, deviceType } = device

  return (
    // Not shrunk, as the previews have a fixed size. The content fills the height below the header.
    <Card
      sx={{
        flexShrink: 0,
        width: TOP_LEVEL_CARD_WIDTH,
        height: TOP_LEVEL_CARD_HEIGHT,
        display: "flex",
        flexDirection: "column",
      }}
    >
      {/* The name is centered between the actions and the content, with 12px on both sides */}
      <Box sx={{ px: 2, pt: 1, pb: 0.5 }}>
        <Stack direction={"row"} justifyContent={"flex-end"} alignItems={"center"}>
          <IdentifyDeviceButton device={device} size={"small"} />
          <RenameDeviceButton device={device} size={"small"} />
          {action && (
            <>
              <Divider orientation={"vertical"} flexItem sx={{ mx: 0.5 }} />
              {action}
            </>
          )}
          <Box sx={{ display: "flex", ml: 1 }}>
            <DeviceInfoIcon device={device} size={"small"} />
          </Box>
        </Stack>
        <DeviceTitle device={device} />
      </Box>
      <CardContent sx={{ flexGrow: 1, minHeight: 0, display: "flex", flexDirection: "column", pt: 1 }}>
        <PreviewCardContent deviceId={deviceId} deviceType={deviceType} />
      </CardContent>
    </Card>
  )
}

/**
 * The name, centered, and cut off when it doesn't fit.
 */
const DeviceTitle = (props: {
  device: DeviceInformation
}) => {
  const { device } = props

  return (
    <Typography
      variant={"h5"}
      sx={{ mt: 1.5, textAlign: "center", overflow: "hidden", textOverflow: "ellipsis", whiteSpace: "nowrap" }}
    >
      {device.deviceName}
    </Typography>
  )
}

/**
 * The preview of a top-level device, which includes its sub-devices.
 *
 * The switch clusters, or sub-devices holding them, are placed in three equally high slots spread over the height of
 * the card, as they are placed the same way on all devices.
 */
const PreviewCardContent = (props: {
  deviceId: string
  deviceType: string
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
    <div className={classes.SlotLayout}>
      {
        subDeviceSlots.map((it, index) => (
          it !== null
            ? <SubDevicePreview key={it.deviceId} subDevice={it} />
            : <DisconnectedSubDevicePreview key={`disconnected-${index}`} slot={index + 1} />
        ))
      }
    </div>
  )
}

/**
 * The caption, centered along the bottom, is the name of the sub-device. Its actions and the info icon, with the details
 * including the type, are shown in the top-right corner, like on the top-level cards.
 */
const SubDevicePreview = (props: {
  subDevice: DeviceInformation,
}) => {
  const { subDevice } = props
  const { deviceId, deviceType, deviceName } = subDevice

  const caption = <span className={classes.SubDevicePreviewCaptionName}>{deviceName}</span>
  const corner = (
    <>
      <IdentifyDeviceButton device={subDevice} size={"compact"} />
      <RenameDeviceButton device={subDevice} size={"compact"} />
      <DeviceInfoIcon device={subDevice} size={"compact"} />
    </>
  )

  switch (deviceType) {
    case "smc1":
      return <TypeSmc1PreviewCardContent deviceId={deviceId} caption={caption} corner={corner} />
    case "smc2":
      return <TypeSmc2PreviewCardContent deviceId={deviceId} caption={caption} corner={corner} />
    case "smc3":
      return <TypeSmc3PreviewCardContent deviceId={deviceId} caption={caption} corner={corner} />
    default:
      return (
        <SubDevicePreviewContainer caption={caption} corner={corner}>
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
 * The caption and the corner content are placed in the bottom and top padding, so they don't change the size of
 * the preview.
 */
const SubDevicePreviewContainer = (props: {
  caption: ReactNode,
  corner?: ReactNode,
  elevation?: number,
  children: ReactNode,
}) => {
  const { caption, corner, elevation = 5, children } = props

  return (
    <Paper elevation={elevation} className={classes.SubDevicePreviewContainer}>
      {children}
      <span className={classes.SubDevicePreviewCaption}>{caption}</span>
      {corner !== undefined && <span className={classes.SubDevicePreviewCorner}>{corner}</span>}
    </Paper>
  )
}

const TypeAPreviewCardContent = (props: {
  deviceId: string,
}) => {
  const { deviceId } = props

  const pushButtonStates = usePushButtonStates(deviceId)
  const pushButtonBindings = usePushButtonBindings(deviceId)

  return (
    // A single panel, with its clusters in the same slots as the sub-devices of type C devices
    <Paper elevation={5} className={`${classes.SlotLayout} ${classes.TypeAPreviewContainer}`}>
      {[1, 5, 9].map(firstSwitchNumber => (
        <div key={firstSwitchNumber} className={classes.TypeASlot}>
          <SwitchStateIndicatorCluster
            firstSwitchNumber={firstSwitchNumber}
            pushButtonStates={pushButtonStates}
            pushButtonBindings={pushButtonBindings}
          />
        </div>
      ))}
    </Paper>
  )
}

const TypeSmc1PreviewCardContent = (props: {
  deviceId: string,
  caption: ReactNode,
  corner: ReactNode,
}) => {
  const { deviceId, caption, corner } = props

  const pushButtonStates = usePushButtonStates(deviceId)
  const pushButtonBindings = usePushButtonBindings(deviceId)

  return (
    <SubDevicePreviewContainer caption={caption} corner={corner}>
      <SwitchStateIndicatorCluster
        firstSwitchNumber={1}
        pushButtonStates={pushButtonStates}
        pushButtonBindings={pushButtonBindings}
      />
    </SubDevicePreviewContainer>
  )
}

const TypeSmc2PreviewCardContent = (props: {
  deviceId: string,
  caption: ReactNode,
  corner: ReactNode,
}) => {
  const { deviceId, caption, corner } = props

  const pushButtonStates = usePushButtonStates(deviceId)
  const pushButtonBindings = usePushButtonBindings(deviceId)

  // 1u switches, so three of them span the same width as two of the 1.5u switches of the other clusters
  return (
    <SubDevicePreviewContainer caption={caption} corner={corner}>
      <SwitchStateIndicatorCluster
        firstSwitchNumber={1}
        rows={2}
        columns={3}
        pushButtonStates={pushButtonStates}
        pushButtonBindings={pushButtonBindings}
      />
    </SubDevicePreviewContainer>
  )
}

const TypeSmc3PreviewCardContent = (props: {
  deviceId: string,
  caption: ReactNode,
  corner: ReactNode,
}) => {
  const { deviceId, caption, corner } = props

  const pushButtonStates = usePushButtonStates(deviceId)
  const rotaryEncoderStates = useRotaryEncoderStates(deviceId, 300)

  return (
    <SubDevicePreviewContainer caption={caption} corner={corner}>
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
  pushButtonBindings: PushButtonBindings,
}) => {
  const { firstSwitchNumber, rows = 2, columns = 2, pushButtonStates, pushButtonBindings } = props

  return (
    <Grid container spacing={1} className={classes.SwitchStateIndicatorCluster}>
      {
        Array.from({ length: rows * columns }, (_, offset) => {
          const switchNumber = firstSwitchNumber + offset
          return (
            <Grid key={switchNumber} size={12 / columns}>
              <SwitchStateIndicator
                switchNumber={switchNumber}
                state={pushButtonStates[switchNumber]}
                binding={pushButtonBindings[switchNumber]}
              />
            </Grid>
          )
        })
      }
    </Grid>
  )
}

/**
 * The switch number is centered on unbound switches. On bound switches it's moved to the top-left corner, making room
 * for the bound macros with their types: the press macro centered, and the long press macro, faded, along the bottom.
 * The names are cut off when they don't fit, so the full names are shown in a tooltip.
 */
const SwitchStateIndicator = (props: {
  switchNumber: number,
  state: PushButtonState | undefined,
  binding: PushButtonBinding | undefined,
}) => {
  const { switchNumber, state, binding } = props
  const pressedMacro = binding?.pressed ?? null
  const longPressedMacro = binding?.longPressed ?? null

  const indicator = (
    <div
      className={classes.SwitchStateIndicator}
      data-pressed={isPushButtonPressed(state)}
      data-long-pressed={state === PushButtonState.LongPressed}
      data-bound={binding !== undefined}
    >
      <span className={classes.SwitchStateIndicatorNumber}>{switchNumber}</span>
      {pressedMacro && <SwitchMacro macro={pressedMacro} />}
      {longPressedMacro && <SwitchMacro macro={longPressedMacro} isLongPress />}
    </div>
  )

  if (binding === undefined) {
    return indicator
  }

  return (
    <Tooltip
      title={
        <>
          {pressedMacro && <div>Press: <strong>{macroPath(pressedMacro)}</strong></div>}
          {longPressedMacro && <div>Long press: <strong>{macroPath(longPressedMacro)}</strong></div>}
        </>
      }
    >
      {indicator}
    </Tooltip>
  )
}

const SwitchMacro = (props: {
  macro: MacroDefinition,
  isLongPress?: boolean,
}) => {
  const { macro, isLongPress = false } = props

  return (
    <span className={classes.SwitchMacro} data-long-press={isLongPress}>
      <MacroTypeVisualization type={macro.type} />
      <span className={classes.SwitchMacroName}>{macro.name}</span>
    </span>
  )
}

function macroPath(macro: MacroDefinition) {
  return macro.directory ? `${macro.directory}/${macro.name}` : macro.name
}
