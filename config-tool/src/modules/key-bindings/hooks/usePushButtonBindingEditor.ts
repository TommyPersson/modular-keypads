import { ClearKeyBindingCommand, SetKeyBindingCommand } from "@src/modules/key-bindings/commands"
import {
  KeyBindingTriggerType,
  type PushButtonEvent,
  type PushButtonKeyBindingTrigger
} from "@src/modules/key-bindings/models"
import type { MacroDefinition } from "@src/modules/macros/models"
import { useCommand } from "@src/utils/commands"
import { useCallback } from "react"

export type UsePushButtonBindingEditorResult = {
  // Binds the macro to the event, or clears the binding of the event when there's no macro
  setBinding: (event: PushButtonEvent, macro: MacroDefinition | null) => Promise<void>
  isPending: boolean
}

/**
 * Changes the macros bound to a push button of a device.
 */
export function usePushButtonBindingEditor(deviceId: string, number: number): UsePushButtonBindingEditorResult {
  const setKeyBindingCommand = useCommand(SetKeyBindingCommand)
  const clearKeyBindingCommand = useCommand(ClearKeyBindingCommand)

  const { executeAsync: setKeyBinding } = setKeyBindingCommand
  const { executeAsync: clearKeyBinding } = clearKeyBindingCommand

  const setBinding = useCallback(async (event: PushButtonEvent, macro: MacroDefinition | null) => {
    const trigger: PushButtonKeyBindingTrigger = { type: KeyBindingTriggerType.PushButton, deviceId, number, event }

    if (macro) {
      await setKeyBinding({ trigger, macroId: macro.id })
    } else {
      await clearKeyBinding({ trigger })
    }
  }, [deviceId, number, setKeyBinding, clearKeyBinding])

  return {
    setBinding,
    isPending: setKeyBindingCommand.isPending || clearKeyBindingCommand.isPending,
  }
}
