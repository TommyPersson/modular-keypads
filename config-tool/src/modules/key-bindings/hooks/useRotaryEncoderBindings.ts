import { useKeyBindingsQuery } from "@src/modules/key-bindings/hooks/useKeyBindingsQuery"
import { KeyBindingTriggerType, RotaryEncoderDirection } from "@src/modules/key-bindings/models"
import { useStoredMacros } from "@src/modules/macros/hooks"
import type { MacroDefinition } from "@src/modules/macros/models"
import { useMemo } from "react"

// The macros bound to a rotary encoder, for each direction it can be rotated in
export type RotaryEncoderBinding = {
  clockwise: MacroDefinition | null
  counterClockwise: MacroDefinition | null
}

// By rotary encoder number, only for the rotary encoders with at least one bound macro
export type RotaryEncoderBindings = { [number: number]: RotaryEncoderBinding }

/**
 * The macros bound to the rotations of the rotary encoders of the given device. Bindings to macros that aren't stored
 * are left out. The bindings to the push buttons of the encoders are push button bindings.
 */
export function useRotaryEncoderBindings(deviceId: string): RotaryEncoderBindings {
  const keyBindingsQuery = useKeyBindingsQuery()
  const { macrosById } = useStoredMacros()

  return useMemo(() => {
    const bindings: RotaryEncoderBindings = {}

    for (const { trigger, macroId } of keyBindingsQuery.data ?? []) {
      const macro = macrosById[macroId]
      if (trigger.type !== KeyBindingTriggerType.RotaryEncoder || trigger.deviceId !== deviceId || !macro) {
        continue
      }

      const binding = bindings[trigger.number] ?? { clockwise: null, counterClockwise: null }
      bindings[trigger.number] = trigger.direction === RotaryEncoderDirection.Clockwise
        ? { ...binding, clockwise: macro }
        : { ...binding, counterClockwise: macro }
    }

    return bindings
  }, [keyBindingsQuery.data, macrosById, deviceId])
}
