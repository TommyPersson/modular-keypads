import * as React from "react"
import { useContext } from "react"

/**
 * An input of a device that macros are bound to: a push button, or a rotary encoder together with its push button,
 * which is numbered separately from the encoder.
 */
export type KeyBindingTarget =
  | { type: "PushButton", deviceId: string, buttonNumber: number }
  | { type: "RotaryEncoder", deviceId: string, encoderNumber: number, buttonNumber: number }

export type KeyBindingSelectionContextValue = {
  // The input whose bindings are edited in the key bindings pane, which is closed while it's null
  selectedTarget: KeyBindingTarget | null
  setSelectedTarget: (value: KeyBindingTarget | null) => void
}

// Without a provider, nothing can be selected, e.g. where the device previews are shown without the pane
export const defaultKeyBindingSelectionContextValue: KeyBindingSelectionContextValue = {
  selectedTarget: null,
  setSelectedTarget: () => {
  },
}

export const KeyBindingSelectionContext = React.createContext(defaultKeyBindingSelectionContextValue)

export function useKeyBindingSelection(): KeyBindingSelectionContextValue {
  return useContext(KeyBindingSelectionContext)
}

// Marks the elements that select an input, so that clicking them selects it, rather than deselecting the selected
// input as clicking elsewhere outside the key bindings pane does
export const keyBindingTargetProps = { "data-key-binding-target": "" }

export function isWithinKeyBindingTarget(element: EventTarget | null) {
  return element instanceof Element && element.closest("[data-key-binding-target]") !== null
}

export function isSelectedPushButton(target: KeyBindingTarget | null, deviceId: string, buttonNumber: number) {
  return target?.type === "PushButton" && target.deviceId === deviceId && target.buttonNumber === buttonNumber
}

export function isSelectedRotaryEncoder(target: KeyBindingTarget | null, deviceId: string, encoderNumber: number) {
  return target?.type === "RotaryEncoder" && target.deviceId === deviceId && target.encoderNumber === encoderNumber
}
