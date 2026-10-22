import { type RefObject, useLayoutEffect, useState } from "react"

/**
 * Whether the content of the element is wider than the element, e.g. so that it can be shown to be cut off. Checked
 * when the element is resized, and after every render, which is when its content changes.
 */
export function useIsOverflowing(ref: RefObject<HTMLElement | null>) {
  const [isOverflowing, setIsOverflowing] = useState(false)

  // After every render, without dependencies. Setting the same value again doesn't render again.
  // eslint-disable-next-line react-hooks/exhaustive-deps
  useLayoutEffect(() => {
    const element = ref.current
    if (element) {
      setIsOverflowing(element.scrollWidth > element.clientWidth)
    }
  })

  useLayoutEffect(() => {
    const element = ref.current
    if (!element) {
      return
    }

    const observer = new ResizeObserver(() => setIsOverflowing(element.scrollWidth > element.clientWidth))
    observer.observe(element)
    return () => observer.disconnect()
  }, [ref])

  return isOverflowing
}
