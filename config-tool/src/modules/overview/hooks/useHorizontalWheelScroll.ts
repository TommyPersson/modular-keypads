import { type RefObject, useEffect } from "react"

// The size of a line, for wheel events that are given in lines rather than pixels
const LINE_HEIGHT_PX = 16
// Wheel events that move at least this far are taken to be from the notches of a mouse wheel, which are animated,
// rather than from a touchpad, whose many small movements are followed directly
const MOUSE_WHEEL_MIN_DELTA_PX = 50
// The part of the remaining distance to scroll that is covered in every 16 ms of the animation
const EASING_PER_FRAME = 0.25

/**
 * Scrolls the container horizontally with the vertical scroll wheel, as long as it can't be scrolled vertically, so
 * that a regular mouse wheel can be used to scroll content that is only wider than the container. Horizontal wheel
 * movement and holding shift are left to the browser, which already scrolls horizontally then, and holding ctrl to
 * its zooming.
 *
 * The notches of a mouse wheel scroll smoothly, easing out towards where they scroll to, which moves further along
 * with every notch. Scrolling some other way meanwhile, e.g. by dragging the scroll bar, stops the animation.
 * Animated here rather than by the browser's smooth scrollBy, which turned out to be jittery.
 */
export function useHorizontalWheelScroll(scrollerRef: RefObject<HTMLElement | null>) {
  useEffect(() => {
    const scroller = scrollerRef.current
    if (!scroller) {
      return
    }

    let target = 0
    // Kept apart from the scroll position, which the browser may round to whole pixels
    let position = 0
    let lastScrollLeft = 0
    let lastTime = 0
    let frame: number | null = null

    const stopAnimation = () => {
      if (frame !== null) {
        cancelAnimationFrame(frame)
        frame = null
      }
    }

    const animate = (time: number) => {
      // Scrolled some other way, which takes over
      if (Math.abs(scroller.scrollLeft - lastScrollLeft) > 1) {
        frame = null
        return
      }

      const remaining = target - position
      if (Math.abs(remaining) < 1) {
        scroller.scrollLeft = target
        frame = null
        return
      }

      const easing = 1 - Math.pow(1 - EASING_PER_FRAME, (time - lastTime) / 16)
      lastTime = time
      position += remaining * easing
      scroller.scrollLeft = position
      lastScrollLeft = scroller.scrollLeft
      frame = requestAnimationFrame(animate)
    }

    const onWheel = (event: WheelEvent) => {
      const canScrollVertically = scroller.scrollHeight > scroller.clientHeight
      const canScrollHorizontally = scroller.scrollWidth > scroller.clientWidth
      const isVertical = Math.abs(event.deltaY) > Math.abs(event.deltaX)
      if (canScrollVertically || !canScrollHorizontally || !isVertical || event.shiftKey || event.ctrlKey) {
        return
      }
      event.preventDefault()

      const delta = event.deltaMode === WheelEvent.DOM_DELTA_LINE ? event.deltaY * LINE_HEIGHT_PX
        : event.deltaMode === WheelEvent.DOM_DELTA_PAGE ? event.deltaY * scroller.clientWidth
          : event.deltaY

      if (Math.abs(delta) < MOUSE_WHEEL_MIN_DELTA_PX) {
        stopAnimation()
        scroller.scrollLeft += delta
        return
      }

      // Continues from where the running animation is heading, so that quick notches add up
      const maxScrollLeft = scroller.scrollWidth - scroller.clientWidth
      const from = frame !== null ? target : scroller.scrollLeft
      target = Math.min(Math.max(from + delta, 0), maxScrollLeft)
      if (frame === null) {
        position = scroller.scrollLeft
        lastScrollLeft = scroller.scrollLeft
        lastTime = performance.now()
        frame = requestAnimationFrame(animate)
      }
    }

    // Not passive, so that the page isn't scrolled too
    scroller.addEventListener("wheel", onWheel, { passive: false })
    return () => {
      scroller.removeEventListener("wheel", onWheel)
      stopAnimation()
    }
  }, [scrollerRef])
}
