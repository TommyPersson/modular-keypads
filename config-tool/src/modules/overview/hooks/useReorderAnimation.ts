import { useCallback, useLayoutEffect, useRef } from "react"

export type ReorderAnimation = {
  // Records where the elements are, to be called right before changing their order
  capturePositions: () => void
  // The ref of the element with the given key, which is moved when the order changes
  elementRef: (key: string) => (element: HTMLElement | null) => void
}

type CapturedPositions = {
  order: string
  lefts: Map<string, number>
}

/**
 * Moves elements horizontally from where they were to where they are when their order changes, rather than letting
 * them jump there. The positions are captured before the change, and the elements are then animated from their old
 * positions to their new ones, once they have been laid out in the new order.
 *
 * Only changes made after capturing the positions are animated, so that e.g. added elements are left to other
 * transitions.
 */
export function useReorderAnimation(keys: string[], durationMs: number): ReorderAnimation {
  const elements = useRef(new Map<string, HTMLElement>())
  const captured = useRef<CapturedPositions | null>(null)

  const order = keys.join("\n")

  const capturePositions = useCallback(() => {
    const lefts = new Map<string, number>()
    elements.current.forEach((element, key) => lefts.set(key, element.getBoundingClientRect().left))
    captured.current = { order, lefts }
  }, [order])

  // On every render, as the first render after capturing is the one with the new order
  useLayoutEffect(() => {
    const previous = captured.current
    if (previous === null || previous.order === order) {
      return
    }
    captured.current = null

    elements.current.forEach((element, key) => {
      const previousLeft = previous.lefts.get(key)
      if (previousLeft === undefined) {
        return
      }

      const offset = previousLeft - element.getBoundingClientRect().left
      if (offset !== 0) {
        element.animate(
          [{ transform: `translateX(${offset}px)` }, { transform: "translateX(0)" }],
          { duration: durationMs, easing: "ease-in-out" },
        )
      }
    })
  })

  const elementRef = useCallback((key: string) => (element: HTMLElement | null) => {
    if (element) {
      elements.current.set(key, element)
    } else {
      elements.current.delete(key)
    }
  }, [])

  return { capturePositions, elementRef }
}
