import { type RefObject, useLayoutEffect } from "react"

// The CSS variable on the container that holds the height of its horizontal scrollbar, or 0px while it has none
export const HORIZONTAL_SCROLLBAR_HEIGHT_VAR = "--horizontal-scrollbar-height"

/**
 * Keeps the height of the container's horizontal scrollbar in a CSS variable on the container, so that its content
 * can make up for the space the scrollbar takes when it appears, e.g. to stay where it is.
 *
 * Set directly on the element rather than through state, as resize observers are notified before the browser paints,
 * so that the content is never shown in between.
 */
export function useHorizontalScrollbarHeight(scrollerRef: RefObject<HTMLElement | null>) {
  useLayoutEffect(() => {
    const scroller = scrollerRef.current
    if (!scroller) {
      return
    }

    // The scrollbar changes the size of the content box, but not of the border box
    const update = () => {
      const height = scroller.offsetHeight - scroller.clientHeight
      scroller.style.setProperty(HORIZONTAL_SCROLLBAR_HEIGHT_VAR, `${height}px`)
    }
    update()

    const observer = new ResizeObserver(update)
    observer.observe(scroller)
    return () => observer.disconnect()
  }, [scrollerRef])
}
