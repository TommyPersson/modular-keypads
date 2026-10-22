import ChevronRightIcon from "@mui/icons-material/ChevronRight"
import CloseIcon from "@mui/icons-material/Close"
import TouchAppOutlinedIcon from "@mui/icons-material/TouchAppOutlined"
import { Box, Divider, IconButton, Paper, Stack, Tooltip, Typography } from "@mui/material"
import { CommandButton } from "@src/modules/common/components"
import {
  FlashButtonIdentificationLightCommand
} from "@src/modules/device/commands/FlashButtonIdentificationLightCommand"
import { useDeviceFacade } from "@src/modules/device/context"
import { useConnectedDevices } from "@src/modules/device/hooks/useConnectedDevices"
import { ListDeviceCapabilitiesQuery } from "@src/modules/device/queries/ListDeviceCapabilitiesQuery"
import {
  usePushButtonBindingEditor,
  usePushButtonBindings,
  useRotaryEncoderBindingEditor,
  useRotaryEncoderBindings
} from "@src/modules/key-bindings/hooks"
import { PushButtonEvent, RotaryEncoderDirection } from "@src/modules/key-bindings/models"
import { useStoredMacros } from "@src/modules/macros/hooks"
import type { MacroDefinition } from "@src/modules/macros/models"
import { MacroAutocomplete } from "@src/modules/macros/ui"
import { useQuery } from "@tanstack/react-query"
import { Fragment, type MouseEvent as ReactMouseEvent, type ReactNode, useCallback, useEffect, useRef } from "react"
import { isWithinKeyBindingTarget, type KeyBindingTarget, useKeyBindingSelection } from "./KeyBindingSelectionContext"

/**
 * Edits the macros bound to the selected input, next to the devices rather than in a dialog, so that the devices can
 * still be used, e.g. to select another input. Always shown, with a hint to select an input while none is. Fills its
 * parent, which places it.
 *
 * The selections are applied right away, so the input only needs to be deselected, if at all. It's deselected by
 * clicking anywhere outside the pane, except on another input, which is selected instead, and when its device is
 * disconnected.
 */
export const KeyBindingsPane = () => {
  const { selectedTarget, setSelectedTarget } = useKeyBindingSelection()

  const connectedDevicesQuery = useConnectedDevices()
  const isDeviceConnected = connectedDevicesQuery.data?.some(it => it.deviceId === selectedTarget?.deviceId)
  useEffect(() => {
    if (selectedTarget !== null && connectedDevicesQuery.data !== undefined && !isDeviceConnected) {
      setSelectedTarget(null)
    }
  }, [selectedTarget, connectedDevicesQuery.data, isDeviceConnected, setSelectedTarget])

  const handleDeselect = useCallback(() => {
    setSelectedTarget(null)
  }, [setSelectedTarget])

  // Clicks within the pane are told apart by React rather than by the DOM, as they include clicks in what the pane
  // shows in portals, like the macro options and the macro dialog. React handles them before they reach the document.
  const clickWithinPane = useRef<Event | null>(null)
  const handleClickCapture = useCallback((event: ReactMouseEvent) => {
    clickWithinPane.current = event.nativeEvent
  }, [])

  useEffect(() => {
    if (selectedTarget === null) {
      return
    }

    const handleClick = (event: MouseEvent) => {
      if (event !== clickWithinPane.current && !isWithinKeyBindingTarget(event.target)) {
        setSelectedTarget(null)
      }
    }

    document.addEventListener("click", handleClick)
    return () => {
      document.removeEventListener("click", handleClick)
    }
  }, [selectedTarget, setSelectedTarget])

  return (
    <Paper
      elevation={1}
      sx={{ height: "100%", borderRadius: 2, p: 2, overflow: "hidden" }}
      onClickCapture={handleClickCapture}
    >
      {selectedTarget !== null
        ? (
          // Keyed, so that the editors don't keep the state of the previously selected input
          <TargetBindingsEditor
            key={`${selectedTarget.deviceId}-${selectedTarget.type}-${selectedTarget.buttonNumber}`}
            target={selectedTarget}
            onDeselect={handleDeselect}
          />
        )
        : <NoTargetSelectedHint />}
    </Paper>
  )
}

const NoTargetSelectedHint = () => (
  <Stack direction={"row"} spacing={1} justifyContent={"center"} alignItems={"center"} sx={{ height: "100%" }}>
    <TouchAppOutlinedIcon color={"disabled"} />
    <Typography color={"text.secondary"}>
      Select a switch or an encoder on a device to edit its key bindings
    </Typography>
  </Stack>
)

const TargetBindingsEditor = (props: {
  target: KeyBindingTarget
  onDeselect: () => void
}) => {
  const { target, onDeselect } = props

  switch (target.type) {
    case "PushButton":
      return <PushButtonBindingsEditor target={target} onDeselect={onDeselect} />
    case "RotaryEncoder":
      return <RotaryEncoderBindingsEditor target={target} onDeselect={onDeselect} />
  }
}

/**
 * Selects the macros bound to a push button, for both a press and a long press.
 */
const PushButtonBindingsEditor = (props: {
  target: Extract<KeyBindingTarget, { type: "PushButton" }>
  onDeselect: () => void
}) => {
  const { target: { deviceId, buttonNumber }, onDeselect } = props

  const { allMacros } = useStoredMacros()
  const binding = usePushButtonBindings(deviceId)[buttonNumber]
  const { setBinding, isPending } = usePushButtonBindingEditor(deviceId, buttonNumber)

  const handlePressSelection = useCallback(async (macro: MacroDefinition | null) => {
    await setBinding(PushButtonEvent.Pressed, macro)
  }, [setBinding])

  const handleLongPressSelection = useCallback(async (macro: MacroDefinition | null) => {
    await setBinding(PushButtonEvent.LongPressed, macro)
  }, [setBinding])

  return (
    <EditorLayout
      title={`Switch ${buttonNumber}`}
      deviceId={deviceId}
      buttonNumber={buttonNumber}
      onDeselect={onDeselect}
      buttonFields={
        <>
          <MacroAutocomplete
            macros={allMacros}
            label={"Press"}
            selected={binding?.pressed ?? null}
            disabled={isPending}
            onSelect={handlePressSelection}
          />
          <MacroAutocomplete
            macros={allMacros}
            label={"Long Press"}
            selected={binding?.longPressed ?? null}
            disabled={isPending}
            onSelect={handleLongPressSelection}
          />
        </>
      }
    />
  )
}

/**
 * Selects the macros bound to a rotary encoder, for both directions of rotation, and to its push button, for both
 * a press and a long press.
 */
const RotaryEncoderBindingsEditor = (props: {
  target: Extract<KeyBindingTarget, { type: "RotaryEncoder" }>
  onDeselect: () => void
}) => {
  const { target: { deviceId, encoderNumber, buttonNumber }, onDeselect } = props

  const { allMacros } = useStoredMacros()

  const rotaryEncoderBinding = useRotaryEncoderBindings(deviceId)[encoderNumber]
  const rotaryEncoderBindingEditor = useRotaryEncoderBindingEditor(deviceId, encoderNumber)
  const { setBinding: setRotaryEncoderBinding } = rotaryEncoderBindingEditor

  const pushButtonBinding = usePushButtonBindings(deviceId)[buttonNumber]
  const pushButtonBindingEditor = usePushButtonBindingEditor(deviceId, buttonNumber)
  const { setBinding: setPushButtonBinding } = pushButtonBindingEditor

  const disabled = rotaryEncoderBindingEditor.isPending || pushButtonBindingEditor.isPending

  const handleCounterClockwiseSelection = useCallback(async (macro: MacroDefinition | null) => {
    await setRotaryEncoderBinding(RotaryEncoderDirection.CounterClockwise, macro)
  }, [setRotaryEncoderBinding])

  const handleClockwiseSelection = useCallback(async (macro: MacroDefinition | null) => {
    await setRotaryEncoderBinding(RotaryEncoderDirection.Clockwise, macro)
  }, [setRotaryEncoderBinding])

  const handlePressSelection = useCallback(async (macro: MacroDefinition | null) => {
    await setPushButtonBinding(PushButtonEvent.Pressed, macro)
  }, [setPushButtonBinding])

  const handleLongPressSelection = useCallback(async (macro: MacroDefinition | null) => {
    await setPushButtonBinding(PushButtonEvent.LongPressed, macro)
  }, [setPushButtonBinding])

  return (
    <EditorLayout
      title={`Encoder ${encoderNumber}`}
      deviceId={deviceId}
      buttonNumber={buttonNumber}
      onDeselect={onDeselect}
      buttonFields={
        <>
          <MacroAutocomplete
            macros={allMacros}
            label={"Press"}
            selected={pushButtonBinding?.pressed ?? null}
            disabled={disabled}
            onSelect={handlePressSelection}
          />
          <MacroAutocomplete
            macros={allMacros}
            label={"Long Press"}
            selected={pushButtonBinding?.longPressed ?? null}
            disabled={disabled}
            onSelect={handleLongPressSelection}
          />
        </>
      }
      rotationFields={
        <>
          <MacroAutocomplete
            macros={allMacros}
            label={"Clockwise"}
            selected={rotaryEncoderBinding?.clockwise ?? null}
            disabled={disabled}
            onSelect={handleClockwiseSelection}
          />
          <MacroAutocomplete
            macros={allMacros}
            label={"Counterclockwise"}
            selected={rotaryEncoderBinding?.counterClockwise ?? null}
            disabled={disabled}
            onSelect={handleCounterClockwiseSelection}
          />
        </>
      }
    />
  )
}

/**
 * The header, with the title and the names of the devices, followed by the macro fields in two columns: the press and
 * long press fields of the push button on the left, and the rotation fields on the right, separated by a divider.
 * Without rotation fields, the push button column is centered, and as wide as with them, so that the fields are sized
 * the same way for all inputs. The pane is equally high for all inputs too.
 */
const EditorLayout = (props: {
  title: string
  deviceId: string
  buttonNumber: number
  onDeselect: () => void
  buttonFields: ReactNode
  rotationFields?: ReactNode
}) => {
  const { title, deviceId, buttonNumber, onDeselect, buttonFields, rotationFields } = props

  return (
    <Stack spacing={1.5}>
      <EditorHeader title={title} deviceId={deviceId} buttonNumber={buttonNumber} onDeselect={onDeselect} />
      <Stack direction={"row"} spacing={2} justifyContent={"center"}>
        <FieldColumn>{buttonFields}</FieldColumn>
        {rotationFields && (
          <>
            <Divider orientation={"vertical"} flexItem />
            <FieldColumn>{rotationFields}</FieldColumn>
          </>
        )}
      </Stack>
    </Stack>
  )
}

// Half of the width that remains next to the divider and the spacing around it, also when there's only one column
const FieldColumn = (props: { children: ReactNode }) => (
  <Stack spacing={1.5} sx={{ flex: "0 1 calc((100% - 33px) / 2)", minWidth: 0 }}>{props.children}</Stack>
)

/**
 * The title, after the name of the device, and the name of its parent device for sub-devices, like a path, with the
 * names cut off when they don't fit. Right after it, a button for flashing the identification light of the push
 * button, to find it on the device, which is disabled for push buttons without a light. In the top-right corner, a
 * button for deselecting the input.
 */
const EditorHeader = (props: {
  title: string
  deviceId: string
  buttonNumber: number
  onDeselect: () => void
}) => {
  const { title, deviceId, buttonNumber, onDeselect } = props

  const deviceFacade = useDeviceFacade()

  const connectedDevicesQuery = useConnectedDevices()
  const device = connectedDevicesQuery.data?.find(it => it.deviceId === deviceId)
  const parentDevice = connectedDevicesQuery.data?.find(it => it.deviceId === device?.parentDeviceId)
  const deviceNames = [parentDevice?.deviceName, device?.deviceName].filter(it => it !== undefined)

  const deviceCapabilitiesQuery = useQuery(ListDeviceCapabilitiesQuery(deviceId, deviceFacade))
  const pushButton = deviceCapabilitiesQuery.data?.find(it => it.type === "PushButton" && it.number === buttonNumber)
  const hasLed = pushButton?.type === "PushButton" && pushButton.hasLed

  return (
    <Stack direction={"row"} alignItems={"center"} spacing={1}>
      <Stack direction={"row"} alignItems={"center"} spacing={0.5} sx={{ minWidth: 0 }}>
        {deviceNames.map((it, index) => (
          <Fragment key={index}>
            <Typography variant={"h6"} color={"text.secondary"} noWrap>{it}</Typography>
            <ChevronRightIcon color={"disabled"} sx={{ flexShrink: 0 }} />
          </Fragment>
        ))}
        <Typography variant={"h6"} sx={{ flexShrink: 0 }}>{title}</Typography>
      </Stack>
      <CommandButton
        command={FlashButtonIdentificationLightCommand}
        args={{ deviceId, buttonNumber }}
        disabled={!hasLed}
        tooltip={pushButton && !hasLed ? "This button does not have an LED indicator" : undefined}
        iconOnly
      />
      <Box sx={{ flexGrow: 1 }} />
      <Tooltip title={"Deselect"}>
        <IconButton onClick={onDeselect}><CloseIcon /></IconButton>
      </Tooltip>
    </Stack>
  )
}
