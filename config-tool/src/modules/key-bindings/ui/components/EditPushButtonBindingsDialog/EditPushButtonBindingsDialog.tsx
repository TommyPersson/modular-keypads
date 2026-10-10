import { Button, Dialog, DialogActions, DialogContent, DialogTitle, Stack, Typography } from "@mui/material"
import { useConnectedDevices } from "@src/modules/device/hooks/useConnectedDevices"
import { usePushButtonBindingEditor, usePushButtonBindings } from "@src/modules/key-bindings/hooks"
import { PushButtonEvent } from "@src/modules/key-bindings/models"
import { MacroAutocomplete } from "@src/modules/key-bindings/ui/components/MacroAutocomplete"
import { useStoredMacros } from "@src/modules/macros/hooks"
import type { MacroDefinition } from "@src/modules/macros/models"
import { useCallback } from "react"

/**
 * Selects the macros bound to a push button of a device, for both a press and a long press. The selections are
 * applied right away, as in the key bindings card, so the dialog only needs to be closed.
 */
export const EditPushButtonBindingsDialog = (props: {
  isOpen: boolean
  deviceId: string
  buttonNumber: number
  onClose: () => void
}) => {
  const { isOpen, deviceId, buttonNumber, onClose } = props

  const connectedDevicesQuery = useConnectedDevices()
  const device = connectedDevicesQuery.data?.find(it => it.deviceId === deviceId)

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
    <Dialog open={isOpen} onClose={onClose} fullWidth maxWidth={"sm"}>
      <DialogTitle>
        Switch {buttonNumber}
        {/* A span, as the title is a heading, which can't contain paragraphs */}
        {device && (
          <Typography variant={"body2"} component={"span"} display={"block"} color={"text.secondary"}>
            {device.deviceName}
          </Typography>
        )}
      </DialogTitle>
      <DialogContent>
        <Stack spacing={2} sx={{ mt: 1 }}>
          <MacroAutocomplete
            macros={allMacros}
            label={"Press Action"}
            selected={binding?.pressed ?? null}
            disabled={isPending}
            onSelect={handlePressSelection}
          />
          <MacroAutocomplete
            macros={allMacros}
            label={"Long Press Action"}
            selected={binding?.longPressed ?? null}
            disabled={isPending}
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
