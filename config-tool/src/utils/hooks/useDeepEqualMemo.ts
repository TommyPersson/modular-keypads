import { dequal } from "dequal"
import { useState } from "react"

export function useDeepEqualMemo<T>(fn: () => T, deps: any[]) {
  const [state, setState] = useState(fn())
  const [oldDeps, setOldDeps] = useState(deps)

  if (!dequal(oldDeps, deps)) {
    setOldDeps(deps)
    setState(fn())
  }

  return state
}