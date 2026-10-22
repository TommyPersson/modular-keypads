import { type ReactNode, useMemo, useState } from "react"
import { KeyBindingSelectionContext, type KeyBindingTarget } from "./KeyBindingSelectionContext"

export const KeyBindingSelectionProvider = (props: {
  children: ReactNode
  // The input that is selected at first, e.g. in stories
  initialSelectedTarget?: KeyBindingTarget | null
}) => {
  const [selectedTarget, setSelectedTarget] = useState<KeyBindingTarget | null>(props.initialSelectedTarget ?? null)

  const value = useMemo(() => ({ selectedTarget, setSelectedTarget }), [selectedTarget])

  return (
    <KeyBindingSelectionContext.Provider value={value}>
      {props.children}
    </KeyBindingSelectionContext.Provider>
  )
}
