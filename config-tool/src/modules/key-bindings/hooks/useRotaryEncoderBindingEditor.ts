import { ClearKeyBindingCommand, SetKeyBindingCommand } from "@src/modules/key-bindings/commands"
import {
  KeyBindingTriggerType,
  type RotaryEncoderDirection,
  type RotaryEncoderKeyBindingTrigger
} from "@src/modules/key-bindings/models"
import type { MacroDefinition } from "@src/modules/macros/models"
import { useCommand } from "@src/utils/commands"
import { useCallback } from "react"

export type UseRotaryEncoderBindingEditorResult = {
  // Binds the macro to the direction, or clears the binding of the direction when there's no macro
  setBinding: (direction: RotaryEncoderDirection, macro: MacroDefinition | null) => Promise<void>
  isPending: boolean
}

/**
 * Changes the macros bound to the rotations of a rotary encoder of a device.
 */
export function useRotaryEncoderBindingEditor(deviceId: string, number: number): UseRotaryEncoderBindingEditorResult {
  const setKeyBindingCommand = useCommand(SetKeyBindingCommand)
  const clearKeyBindingCommand = useCommand(ClearKeyBindingCommand)

  const { executeAsync: setKeyBinding } = setKeyBindingCommand
  const { executeAsync: clearKeyBinding } = clearKeyBindingCommand

  const setBinding = useCallback(async (direction: RotaryEncoderDirection, macro: MacroDefinition | null) => {
    const trigger: RotaryEncoderKeyBindingTrigger = {
      type: KeyBindingTriggerType.RotaryEncoder,
      deviceId,
      number,
      direction,
    }

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
