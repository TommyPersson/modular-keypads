import { Box, Card, CardContent, Divider, Fade, Grid, Paper, Stack, Tooltip, Typography, Zoom } from "@mui/material"
import { type KeyboardEvent, type ReactElement, type ReactNode, useCallback, useState } from "react"
import { TransitionGroup } from "react-transition-group"
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
  usePushButtonBindings,
  useRotaryEncoderBindings
} from "@src/modules/key-bindings/hooks"
import { EditPushButtonBindingsDialog } from "@src/modules/key-bindings/ui/components/EditPushButtonBindingsDialog"
import {
  EditRotaryEncoderBindingsDialog
} from "@src/modules/key-bindings/ui/components/EditRotaryEncoderBindingsDialog"
import type { MacroDefinition } from "@src/modules/macros/models"
import { MacroTypeVisualization } from "@src/modules/macros/ui"
import { DeviceInfoIcon, IdentifyDeviceButton, RenameDeviceButton } from "./DeviceActions"

import classes from "./DevicePreviewCard.module.css"

// Type C devices have a fixed number of sub-device slots, and never report more sub-devices than that
const TYPE_C_SUB_DEVICE_SLOTS = 3

// All top-level devices have the same physical dimensions, so their cards all have the same size
const TOP_LEVEL_CARD_WIDTH = 340
const TOP_LEVEL_CARD_HEIGHT = 670

const SUB_DEVICE_TRANSITION_MS = 300

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
 * e.g. for placing the card, and the info icon in the top-right corner. They're all only shown while the card is
 * hovered. The actions of the sub-devices are in their top-right corners, shown while the sub-device is hovered.
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
      className={classes.TopLevelCard}
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
        <Stack
          direction={"row"}
          justifyContent={"flex-end"}
          alignItems={"center"}
          className={classes.TopLevelHoverActions}
        >
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

  // Each slot cross-fades between its sub-device and the placeholder, so that they're animated when they're connected
  // and disconnected, but not when the card is first shown
  return (
    <div className={classes.SlotLayout}>
      {
        subDeviceSlots.map((it, index) => (
          <TransitionGroup key={index} className={classes.SubDeviceSlot}>
            {
              it !== null
                ? (
                  <SubDeviceSlotTransition key={it.deviceId} zoom>
                    <SubDevicePreview subDevice={it} />
                  </SubDeviceSlotTransition>
                )
                : (
                  <SubDeviceSlotTransition key={"disconnected"}>
                    <DisconnectedSubDevicePreview slot={index + 1} />
                  </SubDeviceSlotTransition>
                )
            }
          </TransitionGroup>
        ))
      }
    </div>
  )
}

/**
 * Fades the content of a slot in and out. Sub-devices also expand from their center when they're connected, and
 * contract towards it when they're disconnected, as the top-level cards do.
 *
 * Receives the transition props from the transition group, which are passed on to the outermost transition.
 */
const SubDeviceSlotTransition = (props: {
  in?: boolean
  appear?: boolean
  zoom?: boolean
  children: ReactNode
}) => {
  const { in: isIn, appear, zoom = false, children, ...transitionProps } = props

  const content = <div className={classes.SubDeviceSlotContent}>{children}</div>

  if (!zoom) {
    return (
      <Fade {...transitionProps} in={isIn} appear={appear} timeout={SUB_DEVICE_TRANSITION_MS}>
        {content}
      </Fade>
    )
  }

  return (
    <Zoom {...transitionProps} in={isIn} appear={appear} timeout={SUB_DEVICE_TRANSITION_MS}>
      <div className={classes.SubDeviceSlotContent}>
        <Fade in={isIn} appear={appear} timeout={SUB_DEVICE_TRANSITION_MS}>
          {content}
        </Fade>
      </div>
    </Zoom>
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
            deviceId={deviceId}
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
        deviceId={deviceId}
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
        deviceId={deviceId}
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
  const pushButtonBindings = usePushButtonBindings(deviceId)
  const rotaryEncoderStates = useRotaryEncoderStates(deviceId, 300)
  const rotaryEncoderBindings = useRotaryEncoderBindings(deviceId)

  const isPressed = isPushButtonPressed(pushButtonStates[1])
  const isLongPressed = pushButtonStates[1] === PushButtonState.LongPressed
  const isRotatingClockwise = rotaryEncoderStates[1]?.clockwise ?? false
  const isRotatingCounterClockwise = rotaryEncoderStates[1]?.counterClockwise ?? false

  const pressedMacro = pushButtonBindings[1]?.pressed ?? null
  const longPressedMacro = pushButtonBindings[1]?.longPressed ?? null
  const clockwiseMacro = rotaryEncoderBindings[1]?.clockwise ?? null
  const counterClockwiseMacro = rotaryEncoderBindings[1]?.counterClockwise ?? null

  // Opened by clicking any part of the encoder or any of the bound macros, which are all edited in the same dialog
  const [isDialogOpen, setIsDialogOpen] = useState(false)

  const handleOpenDialog = useCallback(() => {
    setIsDialogOpen(true)
  }, [])

  const handleDialogClose = useCallback(() => {
    setIsDialogOpen(false)
  }, [])

  // The bound macros are placed around the encoder, in the container rather than in the content
  return (
    <SubDevicePreviewContainer caption={caption} corner={corner}>
      <RotaryEncoderStateIndicator
        encoderNumber={1}
        isPressed={isPressed}
        isLongPressed={isLongPressed}
        isRotatingClockwise={isRotatingClockwise}
        isRotatingCounterClockwise={isRotatingCounterClockwise}
        onClick={handleOpenDialog}
      />
      {counterClockwiseMacro && (
        <BoundMacrosTooltip macros={[["Counterclockwise", counterClockwiseMacro]]}>
          <div
            {...buttonProps("Counterclockwise action", handleOpenDialog)}
            className={classes.RotaryEncoderBindings}
            data-position={"counter-clockwise"}
            data-active={isRotatingCounterClockwise}
          >
            <BoundMacro macro={counterClockwiseMacro} />
          </div>
        </BoundMacrosTooltip>
      )}
      {clockwiseMacro && (
        <BoundMacrosTooltip macros={[["Clockwise", clockwiseMacro]]}>
          <div
            {...buttonProps("Clockwise action", handleOpenDialog)}
            className={classes.RotaryEncoderBindings}
            data-position={"clockwise"}
            data-active={isRotatingClockwise}
          >
            <BoundMacro macro={clockwiseMacro} />
          </div>
        </BoundMacrosTooltip>
      )}
      {(pressedMacro || longPressedMacro) && (
        <BoundMacrosTooltip macros={[["Press", pressedMacro], ["Long press", longPressedMacro]]}>
          <div
            {...buttonProps("Button actions", handleOpenDialog)}
            className={classes.RotaryEncoderBindings}
            data-position={"button"}
            data-active={isPressed}
            data-long-active={isLongPressed}
          >
            {pressedMacro && <BoundMacro macro={pressedMacro} />}
            {longPressedMacro && <BoundMacro macro={longPressedMacro} isLongPress />}
          </div>
        </BoundMacrosTooltip>
      )}
      <EditRotaryEncoderBindingsDialog
        isOpen={isDialogOpen}
        deviceId={deviceId}
        encoderNumber={1}
        buttonNumber={1}
        onClose={handleDialogClose}
      />
    </SubDevicePreviewContainer>
  )
}

/**
 * A rotary encoder with a push button, drawn as the button in the center wrapped by two half rings, which light
 * up when the encoder is rotated in their direction: counter-clockwise on the left, clockwise on the right.
 *
 * Clicking the rings or the button activates it, but clicking the space around them doesn't. With the keyboard, it's
 * focused and activated as a whole.
 *
 * The data attributes are lowercase, as Chrome doesn't restyle SVG elements when mixed-case attributes change.
 */
const RotaryEncoderStateIndicator = (props: {
  encoderNumber: number,
  isPressed: boolean,
  isLongPressed: boolean,
  isRotatingClockwise: boolean,
  isRotatingCounterClockwise: boolean,
  onClick: () => void,
}) => {
  const { encoderNumber, isPressed, isLongPressed, isRotatingClockwise, isRotatingCounterClockwise, onClick } = props

  return (
    // Focused as a div rather than as the drawing, as SVG elements aren't focused as reliably, e.g. in Storybook
    <div {...buttonProps(`Encoder ${encoderNumber}`, onClick)} className={classes.RotaryEncoderStateIndicator}>
      <svg viewBox={"0 0 120 120"}>
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
    </div>
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
 * A cluster of switches (2x2 by default), numbered from firstSwitchNumber left to right, top to bottom. Clicking a
 * switch opens a dialog for selecting its bound macros.
 */
const SwitchStateIndicatorCluster = (props: {
  deviceId: string,
  firstSwitchNumber: number,
  rows?: number,
  columns?: number,
  pushButtonStates: PushButtonStates,
  pushButtonBindings: PushButtonBindings,
}) => {
  const { deviceId, firstSwitchNumber, rows = 2, columns = 2, pushButtonStates, pushButtonBindings } = props

  // One dialog for the cluster. The switch is kept when the dialog is closed, so that it doesn't change while closing.
  const [isDialogOpen, setIsDialogOpen] = useState(false)
  const [dialogSwitchNumber, setDialogSwitchNumber] = useState(firstSwitchNumber)

  const handleSwitchClick = useCallback((switchNumber: number) => {
    setDialogSwitchNumber(switchNumber)
    setIsDialogOpen(true)
  }, [])

  const handleDialogClose = useCallback(() => {
    setIsDialogOpen(false)
  }, [])

  return (
    <>
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
                  onClick={handleSwitchClick}
                />
              </Grid>
            )
          })
        }
      </Grid>
      <EditPushButtonBindingsDialog
        isOpen={isDialogOpen}
        deviceId={deviceId}
        buttonNumber={dialogSwitchNumber}
        onClose={handleDialogClose}
      />
    </>
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
  onClick: (switchNumber: number) => void,
}) => {
  const { switchNumber, state, binding, onClick } = props
  const pressedMacro = binding?.pressed ?? null
  const longPressedMacro = binding?.longPressed ?? null

  const handleClick = useCallback(() => {
    onClick(switchNumber)
  }, [onClick, switchNumber])

  const indicator = (
    <div
      {...buttonProps(`Switch ${switchNumber}`, handleClick)}
      className={classes.SwitchStateIndicator}
      data-pressed={isPushButtonPressed(state)}
      data-long-pressed={state === PushButtonState.LongPressed}
      data-bound={binding !== undefined}
    >
      <span className={classes.SwitchStateIndicatorNumber}>{switchNumber}</span>
      {pressedMacro && <BoundMacro macro={pressedMacro} />}
      {longPressedMacro && <BoundMacro macro={longPressedMacro} isLongPress />}
    </div>
  )

  if (binding === undefined) {
    return indicator
  }

  return (
    <BoundMacrosTooltip macros={[["Press", pressedMacro], ["Long press", longPressedMacro]]}>
      {indicator}
    </BoundMacrosTooltip>
  )
}

/**
 * The name of a bound macro, with its type, cut off when it doesn't fit. Long press macros are faded.
 */
const BoundMacro = (props: {
  macro: MacroDefinition,
  isLongPress?: boolean,
}) => {
  const { macro, isLongPress = false } = props

  return (
    <span className={classes.BoundMacro} data-long-press={isLongPress}>
      <MacroTypeVisualization type={macro.type} />
      <span className={classes.BoundMacroName}>{macro.name}</span>
    </span>
  )
}

/**
 * Shows the full names of the bound macros, which may be cut off in the preview, labelled by what triggers them.
 * The triggers without a bound macro are left out.
 */
const BoundMacrosTooltip = (props: {
  macros: [label: string, macro: MacroDefinition | null][],
  children: ReactElement,
}) => {
  const { macros, children } = props

  return (
    <Tooltip
      title={macros.map(([label, macro]) => (
        macro && <div key={label}>{label}: <strong>{macroPath(macro)}</strong></div>
      ))}
    >
      {children}
    </Tooltip>
  )
}

/**
 * Makes an element act as a button: clicked, focused, and activated with Enter and Space as buttons are, for the
 * previews that don't look like buttons.
 */
function buttonProps(label: string, onActivate: () => void) {
  return {
    role: "button",
    tabIndex: 0,
    "aria-label": label,
    onClick: onActivate,
    onKeyDown: (event: KeyboardEvent) => {
      if (event.key === "Enter" || event.key === " ") {
        event.preventDefault()
        onActivate()
      }
    },
  }
}

function macroPath(macro: MacroDefinition) {
  return macro.directory ? `${macro.directory}/${macro.name}` : macro.name
}
