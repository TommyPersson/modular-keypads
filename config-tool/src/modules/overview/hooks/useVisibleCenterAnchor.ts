import { type RefObject, useLayoutEffect, useRef } from "react"

/**
 * Keeps what is in the middle of the visible part of a horizontally scrolled container in the middle when the right
 * inset of that part changes, e.g. when something on top of the container covers more or less of it. That way, the
 * content that was in view stays in view as far as it fits, rather than ending up under what covers it.
 *
 * The inset is the right padding of the content, which is expected to be animated for the given duration. The scroll
 * position follows the padding on every frame of the animation, so that the content moves along smoothly.
 */
export function useVisibleCenterAnchor(
  scrollerRef: RefObject<HTMLElement | null>,
  contentRef: RefObject<HTMLElement | null>,
  rightInset: number,
  durationMs: number,
) {
  const previousInset = useRef(rightInset)
  // Where the visible center was within the content before the inset changed, for the inset it changed to
  const anchor = useRef<{ inset: number, offset: number } | null>(null)

  // Measured while rendering, like getSnapshotBeforeUpdate, as the DOM still has the previous layout then
  if (rightInset !== previousInset.current && anchor.current?.inset !== rightInset) {
    const scroller = scrollerRef.current
    const content = contentRef.current
    anchor.current = scroller && content
      ? { inset: rightInset, offset: visibleCenter(scroller, content) - content.getBoundingClientRect().left }
      : null
  }

  useLayoutEffect(() => {
    const scroller = scrollerRef.current
    const content = contentRef.current
    const offset = anchor.current?.inset === rightInset ? anchor.current.offset : null
    previousInset.current = rightInset
    anchor.current = null
    if (offset === null || !scroller || !content) {
      return
    }

    const end = performance.now() + durationMs
    let frame = 0
    const follow = () => {
      scroller.scrollLeft += content.getBoundingClientRect().left + offset - visibleCenter(scroller, content)
      if (performance.now() < end) {
        frame = requestAnimationFrame(follow)
      }
    }
    follow()
    // Also once after the animation has ended, so that the final padding is followed too
    const timeout = setTimeout(() => {
      frame = requestAnimationFrame(follow)
    }, durationMs + 50)

    return () => {
      cancelAnimationFrame(frame)
      clearTimeout(timeout)
    }
  }, [scrollerRef, contentRef, rightInset, durationMs])
}

function visibleCenter(scroller: HTMLElement, content: HTMLElement) {
  const paddingRight = parseFloat(getComputedStyle(content).paddingRight)
  return scroller.getBoundingClientRect().left + (scroller.clientWidth - paddingRight) / 2
}
