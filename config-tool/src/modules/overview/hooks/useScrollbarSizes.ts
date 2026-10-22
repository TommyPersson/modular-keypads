import { type RefObject, useLayoutEffect } from "react"

// The CSS variables on the container that hold the height of its horizontal scrollbar and the width of its vertical
// scrollbar, or 0px while it doesn't have them
export const HORIZONTAL_SCROLLBAR_HEIGHT_VAR = "--horizontal-scrollbar-height"
export const VERTICAL_SCROLLBAR_WIDTH_VAR = "--vertical-scrollbar-width"

/**
 * Keeps the sizes of the container's scrollbars in CSS variables on the container, so that its content can make up
 * for the space the scrollbars take when they appear, e.g. to stay where it is.
 *
 * Set directly on the element rather than through state, as resize observers are notified before the browser paints,
 * so that the content is never shown in between.
 */
export function useScrollbarSizes(scrollerRef: RefObject<HTMLElement | null>) {
  useLayoutEffect(() => {
    const scroller = scrollerRef.current
    if (!scroller) {
      return
    }

    // The scrollbars change the size of the content box, but not of the border box
    const update = () => {
      const height = scroller.offsetHeight - scroller.clientHeight
      const width = scroller.offsetWidth - scroller.clientWidth
      scroller.style.setProperty(HORIZONTAL_SCROLLBAR_HEIGHT_VAR, `${height}px`)
      scroller.style.setProperty(VERTICAL_SCROLLBAR_WIDTH_VAR, `${width}px`)
    }
    update()

    const observer = new ResizeObserver(update)
    observer.observe(scroller)
    return () => observer.disconnect()
  }, [scrollerRef])
}
