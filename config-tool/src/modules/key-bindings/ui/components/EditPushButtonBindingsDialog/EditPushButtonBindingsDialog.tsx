import { Button, Dialog, DialogActions, DialogContent, Stack } from "@mui/material"
import { usePushButtonBindingEditor, usePushButtonBindings } from "@src/modules/key-bindings/hooks"
import { PushButtonEvent } from "@src/modules/key-bindings/models"
import { BindingsDialogTitle } from "@src/modules/key-bindings/ui/components/BindingsDialogTitle"
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
      <BindingsDialogTitle title={`Switch ${buttonNumber}`} deviceId={deviceId} buttonNumber={buttonNumber} />
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
