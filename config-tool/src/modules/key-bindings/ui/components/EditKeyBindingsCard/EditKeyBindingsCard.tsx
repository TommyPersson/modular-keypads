import type { AutocompleteInputChangeReason, AutocompleteValue, SelectChangeEvent } from "@mui/material"
import {
  Autocomplete,
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
  Stack,
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableRow,
  TextField,
  Typography
} from "@mui/material"
import { useDeviceFacade } from "@src/modules/device/context"
import type { DeviceInformation, PushButtonCapability, RotaryEncoderCapability } from "@src/modules/device/facade"
import { type PushButtonStates, usePushButtonStates } from "@src/modules/device/hooks/usePushButtonStates"
import { type RotaryEncoderState, useRotaryEncoderStates } from "@src/modules/device/hooks/useRotaryEncoderStates"
import { ListConnectedDevicesQuery } from "@src/modules/device/queries/ListConnectedDevicesQuery"
import { ListDeviceCapabilitiesQuery } from "@src/modules/device/queries/ListDeviceCapabilitiesQuery"
import { ClearKeyBindingCommand } from "@src/modules/key-bindings/commands"
import { SetKeyBindingCommand } from "@src/modules/key-bindings/commands/SetKeyBindingCommand"
import { useStoredMacrosQuery } from "@src/modules/key-bindings/hooks"
import { useKeyBindingsQuery } from "@src/modules/key-bindings/hooks/useKeyBindingsQuery"
import {
  type KeyBinding,
  KeyBindingTriggerType,
  type MacroDefinition,
  type PushButtonKeyBindingTrigger,
  RotaryEncoderDirection,
  type RotaryEncoderKeyBindingTrigger
} from "@src/modules/key-bindings/models"
import { useCommand } from "@src/utils/commands"
import { useQuery } from "@tanstack/react-query"
import { useCallback, useEffect, useMemo, useState } from "react"

import classes from "./EditKeyBindingsCard.module.css"

const highlightTextColor = "info.contrastText"
const highlightBackgroundColor = "info.light"

export const EditKeyBindingsCard = () => {
  const deviceFacade = useDeviceFacade()

  const devicesQuery = useQuery(ListConnectedDevicesQuery(deviceFacade))
  const devices = devicesQuery.data ?? []

  const [selectedDeviceId, setSelectedDeviceId] = useState<string | null>(devices?.[0]?.deviceId ?? null)

  const deviceCapabilitiesQuery = useQuery(ListDeviceCapabilitiesQuery(selectedDeviceId ?? "", deviceFacade))
  const deviceCapabilities = deviceCapabilitiesQuery.data ?? []

  const pushButtons = deviceCapabilities.filter(it => it.type === "PushButton")
  const pushButtonStates = usePushButtonStates(selectedDeviceId ?? "")

  const rotaryEncoders = deviceCapabilities.filter(it => it.type === "RotaryEncoder")

  const storedMacrosQuery = useStoredMacrosQuery()
  const macros = storedMacrosQuery.data ?? []

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
        macros={macros}
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
      <Stack spacing={2}>
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
      </Stack>
    </CardContent>
  )
}

const PushButtonsSection = (props: {
  deviceId: string
  pushButtons: PushButtonCapability[]
  pushButtonStates: PushButtonStates
  macros: MacroDefinition[]
}) => {
  const { deviceId, pushButtons, pushButtonStates, macros } = props

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
            </TableRow>
          </TableHead>
          <TableBody>
            {pushButtons.map(it => (
              <PushButtonBindingRow
                key={it.number}
                deviceId={deviceId}
                pushButton={it}
                isPressed={pushButtonStates[it.number] ?? false}
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
  isPressed: boolean
  macros: MacroDefinition[]
  keyBindings: KeyBinding[]
}) => {
  const { deviceId, pushButton, isPressed, macros, keyBindings } = props

  const backgroundColor = isPressed ? highlightBackgroundColor : undefined
  const color = isPressed ? highlightTextColor : undefined

  const trigger: PushButtonKeyBindingTrigger = useMemo(() => ({
    type: KeyBindingTriggerType.PushButton,
    deviceId: deviceId,
    number: pushButton.number,
  }), [deviceId, pushButton])

  const keyBinding = keyBindings.find(it => it.trigger.type === "PushButton" && it.trigger.number === pushButton.number && it.trigger.deviceId === deviceId)
  const boundMacro = macros.find(it => it.id === keyBinding?.macroId) ?? null

  const setKeyBindingCommand = useCommand(SetKeyBindingCommand)
  const clearKeyBindingCommand = useCommand(ClearKeyBindingCommand)

  const handleMacroSelection = useCallback(async (macro: MacroDefinition | null) => {
    if (!macro) {
      await clearKeyBindingCommand.executeAsync({ trigger })
    } else {
      const macroId = macro.id

      await setKeyBindingCommand.executeAsync({ trigger, macroId })
    }
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [deviceId, pushButton])

  return (
    <TableRow hover sx={{ backgroundColor, color }}>
      <TableCell sx={{ color }}>
        <strong>{pushButton.number}</strong>
      </TableCell>
      <TableCell sx={{ color }}>
        <MacroAutocomplete
          macros={macros}
          label={"Press Action"}
          selected={boundMacro}
          onSelect={handleMacroSelection}
        />
      </TableCell>
    </TableRow>
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

  const setKeyBindingCommand = useCommand(SetKeyBindingCommand)
  const clearKeyBindingCommand = useCommand(ClearKeyBindingCommand)

  const handleMacroSelection = useCallback(async (macro: MacroDefinition | null, direction: RotaryEncoderDirection) => {
    const trigger: RotaryEncoderKeyBindingTrigger = {
      type: KeyBindingTriggerType.RotaryEncoder,
      deviceId: deviceId,
      number: rotaryEncoder.number,
      direction: direction,
    }

    if (!macro) {
      await clearKeyBindingCommand.executeAsync({ trigger })
    } else {
      const macroId = macro.id

      await setKeyBindingCommand.executeAsync({ trigger, macroId })
    }
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [deviceId, rotaryEncoder])

  const handleMacroSelectionCW = useCallback(async (macro: MacroDefinition | null) => {
    await handleMacroSelection(macro, RotaryEncoderDirection.Clockwise)
  }, [handleMacroSelection])

  const handleMacroSelectionCCW = useCallback(async (macro: MacroDefinition | null) => {
    await handleMacroSelection(macro, RotaryEncoderDirection.CounterClockwise)
  }, [handleMacroSelection])

  return (
    <>
      <TableRow hover sx={{ color: cwColor, backgroundColor: cwBackgroundColor }}>
        <TableCell>
          <strong>{rotaryEncoder.number}</strong>
        </TableCell>
        <TableCell>
          <MacroAutocomplete
            macros={macros}
            label={"Clockwise Action"}
            selected={boundMacroCW}
            onSelect={handleMacroSelectionCW}
          />
        </TableCell>
      </TableRow>
      <TableRow hover sx={{ color: ccwColor, backgroundColor: ccwBackgroundColor }}>
        <TableCell></TableCell>
        <TableCell>
          <MacroAutocomplete
            macros={macros}
            label={"Counterclockwise Action"}
            selected={boundMacroCCW}
            onSelect={handleMacroSelectionCCW}
          />
        </TableCell>
      </TableRow>
    </>
  )
}

const MacroItemVisualization = (props: {
  macro: MacroDefinition
}) => {
  const { macro } = props

  return (
    <Stack direction={"row"} gap={2} alignItems={"baseline"}>
      <Typography style={{ flex: 1 }}>{macro.id}. {macro.name}</Typography>
      <Typography variant={"caption"}>{macro.type}</Typography>
    </Stack>
  )
}

const MacroAutocomplete = (props: {
  macros: MacroDefinition[]
  label: string
  selected: MacroDefinition | null
  onSelect?: (macro: MacroDefinition | null) => void
}) => {
  const { macros, label, selected, onSelect } = props

  const handleChange = useCallback((_: any, value: AutocompleteValue<MacroDefinition, unknown, unknown, unknown>) => {
    onSelect?.(value)
  }, [onSelect])

  const handleInputChange = useCallback((_: any, __: any, reason: AutocompleteInputChangeReason) => {
    if (reason === "clear") {
      onSelect?.(null)
    }
  }, [onSelect])

  return (
    <Autocomplete
      size={"small"}
      renderInput={(params) => (
        <TextField variant={"filled"} {...params} label={label} />
      )}
      renderOption={(props, option) => (
        <Box component={"li"} {...props} key={props.key}>
          <MacroItemVisualization macro={option} />
        </Box>
      )}
      renderValue={(option) => (
        <MacroItemVisualization macro={option} />
      )}
      options={macros}
      getOptionLabel={option => option.name + option.id}
      value={selected}
      onChange={handleChange}
      onInputChange={handleInputChange}
    />
  )
}