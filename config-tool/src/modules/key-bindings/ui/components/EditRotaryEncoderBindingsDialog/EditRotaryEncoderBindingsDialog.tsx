import { Button, Dialog, DialogActions, DialogContent, DialogTitle, Stack, Typography } from "@mui/material"
import { useConnectedDevices } from "@src/modules/device/hooks/useConnectedDevices"
import {
  usePushButtonBindingEditor,
  usePushButtonBindings,
  useRotaryEncoderBindingEditor,
  useRotaryEncoderBindings
} from "@src/modules/key-bindings/hooks"
import { PushButtonEvent, RotaryEncoderDirection } from "@src/modules/key-bindings/models"
import { MacroAutocomplete } from "@src/modules/key-bindings/ui/components/MacroAutocomplete"
import { useStoredMacros } from "@src/modules/macros/hooks"
import type { MacroDefinition } from "@src/modules/macros/models"
import { useCallback } from "react"

/**
 * Selects the macros bound to a rotary encoder of a device, for both directions of rotation, and to its push button,
 * for both a press and a long press. The selections are applied right away, as in the key bindings card, so the
 * dialog only needs to be closed.
 */
export const EditRotaryEncoderBindingsDialog = (props: {
  isOpen: boolean
  deviceId: string
  encoderNumber: number
  // The push button of the encoder, which is numbered separately from the encoder
  buttonNumber: number
  onClose: () => void
}) => {
  const { isOpen, deviceId, encoderNumber, buttonNumber, onClose } = props

  const connectedDevicesQuery = useConnectedDevices()
  const device = connectedDevicesQuery.data?.find(it => it.deviceId === deviceId)

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
    <Dialog open={isOpen} onClose={onClose} fullWidth maxWidth={"sm"}>
      <DialogTitle>
        Encoder {encoderNumber}
        {/* A span, as the title is a heading, which can't contain paragraphs */}
        {device && (
          <Typography variant={"body2"} component={"span"} display={"block"} color={"text.secondary"}>
            {device.deviceName}
          </Typography>
        )}
      </DialogTitle>
      <DialogContent>
        <Stack spacing={2} sx={{ mt: 1 }}>
          <Typography variant={"subtitle2"}>Rotation</Typography>
          <MacroAutocomplete
            macros={allMacros}
            label={"Counterclockwise Action"}
            selected={rotaryEncoderBinding?.counterClockwise ?? null}
            disabled={disabled}
            onSelect={handleCounterClockwiseSelection}
          />
          <MacroAutocomplete
            macros={allMacros}
            label={"Clockwise Action"}
            selected={rotaryEncoderBinding?.clockwise ?? null}
            disabled={disabled}
            onSelect={handleClockwiseSelection}
          />
          <Typography variant={"subtitle2"}>Button</Typography>
          <MacroAutocomplete
            macros={allMacros}
            label={"Press Action"}
            selected={pushButtonBinding?.pressed ?? null}
            disabled={disabled}
            onSelect={handlePressSelection}
          />
          <MacroAutocomplete
            macros={allMacros}
            label={"Long Press Action"}
            selected={pushButtonBinding?.longPressed ?? null}
            disabled={disabled}
            onSelect={handleLongPressSelection}
          />
        </Stack>
      </DialogContent>
      <DialogActions>
        <Button onClick={onClose}>Close</Button>
      </DialogActions>
    </Dialog>
  )
}
