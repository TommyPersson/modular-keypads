import { type RefObject, useLayoutEffect, useRef } from "react"

/**
 * Keeps what is in the middle of the visible part of a horizontally scrolled container in the middle when the left or
 * right inset of that part changes, e.g. when something on top of the container covers more or less of it. That way,
 * the content that was in view stays in view as far as it fits, rather than ending up under what covers it.
 *
 * The insets are the left and right padding of the content, which are expected to be animated for the given duration.
 * The scroll position follows the padding on every frame of the animation, so that the content moves along smoothly.
 */
export function useVisibleCenterAnchor(
  scrollerRef: RefObject<HTMLElement | null>,
  contentRef: RefObject<HTMLElement | null>,
  leftInset: number,
  rightInset: number,
  durationMs: number,
) {
  // Both insets in one value, so that a change of either is noticed
  const insets = `${leftInset} ${rightInset}`

  const previousInsets = useRef(insets)
  // Where the visible center was within the content before the insets changed, for the insets they changed to
  const anchor = useRef<{ insets: string, offset: number } | null>(null)

  // Measured while rendering, like getSnapshotBeforeUpdate, as the DOM still has the previous layout then
  if (insets !== previousInsets.current && anchor.current?.insets !== insets) {
    const scroller = scrollerRef.current
    const content = contentRef.current
    anchor.current = scroller && content
      ? { insets, offset: visibleCenter(scroller, content) - contentStart(content) }
      : null
  }

  useLayoutEffect(() => {
    const scroller = scrollerRef.current
    const content = contentRef.current
    const offset = anchor.current?.insets === insets ? anchor.current.offset : null
    previousInsets.current = insets
    anchor.current = null
    if (offset === null || !scroller || !content) {
      return
    }

    const end = performance.now() + durationMs
    let frame = 0
    const follow = () => {
      scroller.scrollLeft += contentStart(content) + offset - visibleCenter(scroller, content)
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
  }, [scrollerRef, contentRef, insets, durationMs])
}

// Where the content starts, within the left padding, which changes with the left inset, so that the offset of the
// visible center doesn't change with it
function contentStart(content: HTMLElement) {
  return content.getBoundingClientRect().left + parseFloat(getComputedStyle(content).paddingLeft)
}

function visibleCenter(scroller: HTMLElement, content: HTMLElement) {
  const { paddingLeft, paddingRight } = getComputedStyle(content)
  return scroller.getBoundingClientRect().left
    + (parseFloat(paddingLeft) + scroller.clientWidth - parseFloat(paddingRight)) / 2
}
