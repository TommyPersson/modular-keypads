import { useMutationObserver } from "@src/utils/hooks/useMutationObserver"
import { useCallback, useState } from "react"

export function useIsAnyModalOpen(): boolean {
  const [state, setState] = useState<boolean>(isAnyModalOpen())

  const handleBodyMutation = useCallback(() => {
    setState(isAnyModalOpen())
  }, [setState])

  useMutationObserver(
    document.body,
    { childList: true },
    handleBodyMutation
  )

  return state
}

const modalClassName = "MuiDialog-root"

function isAnyModalOpen(): boolean {
  return document.getElementsByClassName(modalClassName).length > 0
}