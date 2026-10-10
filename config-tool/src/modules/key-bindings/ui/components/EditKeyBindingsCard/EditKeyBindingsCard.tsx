import {
  Box,
  Card,
  CardContent,
  CardHeader,
  Divider,
  FormControl,
  InputLabel,
  ListItemText,
  MenuItem,
  Select,
  type SelectChangeEvent,
  Stack,
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableRow,
  Typography
} from "@mui/material"
import { CommandButton } from "@src/modules/common/components"
import {
  FlashButtonIdentificationLightCommand
} from "@src/modules/device/commands/FlashButtonIdentificationLightCommand"
import {
  FlashDeviceIdentificationLightsCommand
} from "@src/modules/device/commands/FlashDeviceIdentificationLightsCommand"
import { useDeviceFacade } from "@src/modules/device/context"
import {
  isPushButtonPressed,
  PushButtonState,
  type PushButtonStates,
  usePushButtonStates
} from "@src/modules/device/hooks/usePushButtonStates"
import { type RotaryEncoderState, useRotaryEncoderStates } from "@src/modules/device/hooks/useRotaryEncoderStates"
import type { DeviceInformation, PushButtonCapability, RotaryEncoderCapability } from "@src/modules/device/models"
import { useConnectedDevices } from "@src/modules/device/hooks/useConnectedDevices"
import { ListDeviceCapabilitiesQuery } from "@src/modules/device/queries/ListDeviceCapabilitiesQuery"
import { usePushButtonBindingEditor, useRotaryEncoderBindingEditor } from "@src/modules/key-bindings/hooks"
import { useKeyBindingsQuery } from "@src/modules/key-bindings/hooks/useKeyBindingsQuery"
import {
  type KeyBinding,
  KeyBindingTriggerType,
  PushButtonEvent,
  RotaryEncoderDirection
} from "@src/modules/key-bindings/models"
import { MacroAutocomplete } from "@src/modules/key-bindings/ui/components/MacroAutocomplete"
import { useStoredMacros, type UseStoredMacrosResult } from "@src/modules/macros/hooks"
import type { MacroDefinition } from "@src/modules/macros/models"
import { useQuery } from "@tanstack/react-query"
import { useCallback, useEffect, useState } from "react"

import classes from "./EditKeyBindingsCard.module.css"

const highlightTextColor = "info.contrastText"
const highlightBackgroundColor = "info.light"

export const EditKeyBindingsCard = () => {
  const deviceFacade = useDeviceFacade()

  const devicesQuery = useConnectedDevices()
  const devices = devicesQuery.data ?? []

  const [selectedDeviceId, setSelectedDeviceId] = useState<string | null>(devices?.[0]?.deviceId ?? null)

  const deviceCapabilitiesQuery = useQuery(ListDeviceCapabilitiesQuery(selectedDeviceId ?? "", deviceFacade))
  const deviceCapabilities = deviceCapabilitiesQuery.data ?? []

  const pushButtons = deviceCapabilities.filter(it => it.type === "PushButton")
  const pushButtonStates = usePushButtonStates(selectedDeviceId ?? "")

  const rotaryEncoders = deviceCapabilities.filter(it => it.type === "RotaryEncoder")

  const storedMacros = useStoredMacros()
  const macros = storedMacros.allMacros

  return (
    <Card>
      <CardHeader title={"Key Bindings"} subheader={"Bind pre-defined macros to specific buttons on your devices"} />
      <DeviceSelectionSection
        selectedDeviceId={selectedDeviceId}
        onSelectionChange={setSelectedDeviceId}
        devices={devices}
      />
      <PushButtonsSection
        deviceId={selectedDeviceId ?? ""}
        pushButtons={pushButtons}
        pushButtonStates={pushButtonStates}
        allMacros={storedMacros.allMacros}
        macrosByDirectory={storedMacros.macrosByDirectoryList}
        macros={storedMacros}
      />
      <RotaryEncodersSection
        deviceId={selectedDeviceId ?? ""}
        rotaryEncoders={rotaryEncoders}
        macros={macros}
      />
    </Card>
  )
}

const DeviceSelectionSection = (props: {
  selectedDeviceId: string | null
  onSelectionChange: (deviceId: string | null) => void
  devices: DeviceInformation[]
}) => {
  const { selectedDeviceId, onSelectionChange, devices } = props

  const handleSelectedDeviceIdChanged = useCallback((e: SelectChangeEvent<string | null>) => {
    onSelectionChange(e.target.value)
  }, [onSelectionChange])

  useEffect(() => {
    onSelectionChange(devices[0]?.deviceId ?? null)
  }, [devices, onSelectionChange])

  return (
    <CardContent>
      <Stack spacing={2} direction={"row"} alignItems={"center"}>
        <FormControl fullWidth>
          <InputLabel>Device</InputLabel>
          <Select
            label={"Device"}
            value={selectedDeviceId ?? ""}
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
        <CommandButton
          command={FlashDeviceIdentificationLightsCommand}
          args={{ deviceId: selectedDeviceId ?? "" }}
          iconOnly
        />
      </Stack>
    </CardContent>
  )
}

const PushButtonsSection = (props: {
  deviceId: string
  pushButtons: PushButtonCapability[]
  pushButtonStates: PushButtonStates
  allMacros: MacroDefinition[]
  macrosByDirectory: [string, MacroDefinition[]][]
  macros: UseStoredMacrosResult
}) => {
  const { deviceId, pushButtons, pushButtonStates, allMacros, macrosByDirectory, macros } = props

  const keyBindingsQuery = useKeyBindingsQuery()
  const keyBindings = keyBindingsQuery.data ?? []

  if (pushButtons.length === 0) {
    return null
  }

  return (
    <>
      <Divider />
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
              <TableCell style={{ width: 100, whiteSpace: "nowrap" }}>Button #</TableCell>
              <TableCell>Binding</TableCell>
              <TableCell style={{ width: 0 }}></TableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            {pushButtons.map(it => (
              <PushButtonBindingRow
                key={it.number}
                deviceId={deviceId}
                pushButton={it}
                state={pushButtonStates[it.number]}
                allMacros={allMacros}
                macrosByDirectory={macrosByDirectory}
                macros={macros}
                keyBindings={keyBindings}
              />
            ))}
          </TableBody>
        </Table>
      </CardContent>
    </>
  )
}

const PushButtonBindingRow = (props: {
  deviceId: string
  pushButton: PushButtonCapability
  state: PushButtonState | undefined
  allMacros: MacroDefinition[]
  macrosByDirectory: [string, MacroDefinition[]][]
  macros: UseStoredMacrosResult
  keyBindings: KeyBinding[]
}) => {
  const { deviceId, pushButton, state, allMacros, macros, keyBindings } = props

  const isPressed = isPushButtonPressed(state)
  const backgroundColor = isPressed ? highlightBackgroundColor : undefined
  const color = isPressed ? highlightTextColor : undefined

  const isLongPressed = state === PushButtonState.LongPressed
  const longPressBackgroundColor = isLongPressed ? highlightBackgroundColor : undefined
  const longPressColor = isLongPressed ? highlightTextColor : undefined

  const findBoundMacro = (state: PushButtonEvent) => {
    const keyBinding = keyBindings.find(it =>
      it.trigger.type === KeyBindingTriggerType.PushButton
      && it.trigger.number === pushButton.number
      && it.trigger.deviceId === deviceId
      && it.trigger.event === state
    )
    return allMacros.find(it => it.id === keyBinding?.macroId) ?? null
  }

  const boundMacro = findBoundMacro(PushButtonEvent.Pressed)
  const boundMacroLongPress = findBoundMacro(PushButtonEvent.LongPressed)

  const { setBinding, isPending: disabled } = usePushButtonBindingEditor(deviceId, pushButton.number)

  const handleMacroSelectionPress = useCallback(async (macro: MacroDefinition | null) => {
    await setBinding(PushButtonEvent.Pressed, macro)
  }, [setBinding])

  const handleMacroSelectionLongPress = useCallback(async (macro: MacroDefinition | null) => {
    await setBinding(PushButtonEvent.LongPressed, macro)
  }, [setBinding])

  return (
    <>
      <TableRow hover sx={{}}>
        <TableCell>
          <strong>{pushButton.number}</strong>
        </TableCell>
        <TableCell sx={{ color }}>
          <Stack direction={"row"} spacing={2}>
            <Box sx={{ backgroundColor, color }} flex={1}>
              <MacroAutocomplete
                macros={macros.allMacros}
                label={"Press Action"}
                selected={boundMacro}
                disabled={disabled}
                onSelect={handleMacroSelectionPress}
              />
            </Box>
            <Box sx={{ backgroundColor: longPressBackgroundColor, color: longPressColor }} flex={1}>
              <MacroAutocomplete
                macros={macros.allMacros}
                label={"Long Press Action"}
                selected={boundMacroLongPress}
                disabled={disabled}
                onSelect={handleMacroSelectionLongPress}
              />
            </Box>
          </Stack>
        </TableCell>
        <TableCell>
          <CommandButton
            command={FlashButtonIdentificationLightCommand}
            args={{ deviceId, buttonNumber: pushButton.number }}
            disabled={!pushButton.hasLed}
            tooltip={!pushButton.hasLed ? "This button does not have an LED indicator" : undefined}
            iconOnly
          />
        </TableCell>
      </TableRow>
    </>
  )
}

const RotaryEncodersSection = (props: {
  deviceId: string
  rotaryEncoders: RotaryEncoderCapability[],
  macros: MacroDefinition[]
}) => {
  const { deviceId, rotaryEncoders, macros } = props

  const rotaryEncoderStates = useRotaryEncoderStates(deviceId)

  const keyBindingsQuery = useKeyBindingsQuery()
  const keyBindings = keyBindingsQuery.data ?? []

  if (rotaryEncoders.length === 0) {
    return null
  }

  return (
    <>
      <Divider />
      <CardContent>
        <Typography variant={"subtitle2"}>
          Rotary Encoders
        </Typography>
        <Typography variant={"body1"}>
          You can rotate the encoders on your device to briefly highlight them in the list.
        </Typography>
      </CardContent>
      <CardContent style={{ paddingLeft: 0, paddingRight: 0 }}>
        <Table size={"small"}>
          <TableHead>
            <TableRow>
              <TableCell style={{ width: 100, whiteSpace: "nowrap" }}>Encoder #</TableCell>
              <TableCell>Binding</TableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            {rotaryEncoders.map(it => (
              <RotaryEncoderBindingsRow
                key={it.number}
                deviceId={deviceId}
                rotaryEncoder={it}
                state={rotaryEncoderStates[it.number] ?? {}}
                macros={macros}
                keyBindings={keyBindings}
              />
            ))}
          </TableBody>
        </Table>
      </CardContent>
    </>
  )
}

const RotaryEncoderBindingsRow = (props: {
  deviceId: string
  rotaryEncoder: RotaryEncoderCapability
  state: RotaryEncoderState
  macros: MacroDefinition[]
  keyBindings: KeyBinding[]
}) => {
  const { deviceId, rotaryEncoder, state, macros, keyBindings } = props

  const ccwBackgroundColor = state.counterClockwise ? highlightBackgroundColor : undefined
  const ccwColor = state.counterClockwise ? highlightTextColor : undefined

  const cwBackgroundColor = state.clockwise ? highlightBackgroundColor : undefined
  const cwColor = state.clockwise ? highlightTextColor : undefined

  const keyBindingCW = keyBindings.find(it =>
    it.trigger.type === KeyBindingTriggerType.RotaryEncoder
    && it.trigger.number === rotaryEncoder.number
    && it.trigger.deviceId === deviceId
    && it.trigger.direction === RotaryEncoderDirection.Clockwise
  )

  const keyBindingCCW = keyBindings.find(it =>
    it.trigger.type === KeyBindingTriggerType.RotaryEncoder
    && it.trigger.number === rotaryEncoder.number
    && it.trigger.deviceId === deviceId
    && it.trigger.direction === RotaryEncoderDirection.CounterClockwise
  )

  const boundMacroCW = macros.find(it => it.id === keyBindingCW?.macroId) ?? null
  const boundMacroCCW = macros.find(it => it.id === keyBindingCCW?.macroId) ?? null

  const { setBinding, isPending: disabled } = useRotaryEncoderBindingEditor(deviceId, rotaryEncoder.number)

  const handleMacroSelectionCW = useCallback(async (macro: MacroDefinition | null) => {
    await setBinding(RotaryEncoderDirection.Clockwise, macro)
  }, [setBinding])

  const handleMacroSelectionCCW = useCallback(async (macro: MacroDefinition | null) => {
    await setBinding(RotaryEncoderDirection.CounterClockwise, macro)
  }, [setBinding])

  return (
    <>
      <TableRow hover>
        <TableCell>
          <strong>{rotaryEncoder.number}</strong>
        </TableCell>
        <TableCell>
          <Stack direction={"row"} spacing={2}>
            <Box sx={{ color: ccwColor, backgroundColor: ccwBackgroundColor }} flex={1}>
              <MacroAutocomplete
                macros={macros}
                label={"Counterclockwise Action"}
                selected={boundMacroCCW}
                disabled={disabled}
                onSelect={handleMacroSelectionCCW}
              />
            </Box>
            <Box sx={{  color: cwColor, backgroundColor: cwBackgroundColor }} flex={1}>
              <MacroAutocomplete
                macros={macros}
                label={"Clockwise Action"}
                selected={boundMacroCW}
                disabled={disabled}
                onSelect={handleMacroSelectionCW}
              />
            </Box>
          </Stack>
        </TableCell>
      </TableRow>
    </>
  )
}
