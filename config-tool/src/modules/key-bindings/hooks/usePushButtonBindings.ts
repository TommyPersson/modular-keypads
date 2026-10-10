import { useKeyBindingsQuery } from "@src/modules/key-bindings/hooks/useKeyBindingsQuery"
import { KeyBindingTriggerType, PushButtonEvent } from "@src/modules/key-bindings/models"
import { useStoredMacros } from "@src/modules/macros/hooks"
import type { MacroDefinition } from "@src/modules/macros/models"
import { useMemo } from "react"

// The macros bound to a push button, for each event that can trigger one
export type PushButtonBinding = {
  pressed: MacroDefinition | null
  longPressed: MacroDefinition | null
}

// By push button number, only for the push buttons with at least one bound macro
export type PushButtonBindings = { [number: number]: PushButtonBinding }

/**
 * The macros bound to the push buttons of the given device. Bindings to macros that aren't stored are left out.
 */
export function usePushButtonBindings(deviceId: string): PushButtonBindings {
  const keyBindingsQuery = useKeyBindingsQuery()
  const { macrosById } = useStoredMacros()

  return useMemo(() => {
    const bindings: PushButtonBindings = {}

    for (const { trigger, macroId } of keyBindingsQuery.data ?? []) {
      const macro = macrosById[macroId]
      if (trigger.type !== KeyBindingTriggerType.PushButton || trigger.deviceId !== deviceId || !macro) {
        continue
      }

      const binding = bindings[trigger.number] ?? { pressed: null, longPressed: null }
      bindings[trigger.number] = trigger.event === PushButtonEvent.LongPressed
        ? { ...binding, longPressed: macro }
        : { ...binding, pressed: macro }
    }

    return bindings
  }, [keyBindingsQuery.data, macrosById, deviceId])
}
