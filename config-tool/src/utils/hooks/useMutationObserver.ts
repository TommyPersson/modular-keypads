import { useDeepEqualMemo } from "@src/utils/hooks/useDeepEqualMemo"
import { useEffect } from "react"

export function useMutationObserver(
  target: Node,
  options: MutationObserverInit,
  callback: MutationCallback
) {
  const optionsMemo = useDeepEqualMemo(() => options, [options])

  useEffect(() => {
    const observer = new MutationObserver(callback)

    observer.observe(target, optionsMemo)

    return () => {
      observer.disconnect()
    }
  }, [target, optionsMemo, callback])
}