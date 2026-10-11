import ChevronLeftIcon from "@mui/icons-material/ChevronLeft"
import ChevronRightIcon from "@mui/icons-material/ChevronRight"
import { Box, Collapse, Fade, IconButton, Stack, Tooltip, useTheme, Zoom } from "@mui/material"
import { TopLevelDevicePreviewCard } from "../DevicePreviewCard"
import { useConnectedDevices } from "@src/modules/device/hooks/useConnectedDevices"
import { useDeviceOrder } from "@src/modules/overview/hooks/useDeviceOrder"
import { useReorderAnimation } from "@src/modules/overview/hooks/useReorderAnimation"
import { useHorizontalWheelScroll } from "@src/modules/overview/hooks/useHorizontalWheelScroll"
import { useVisibleCenterAnchor } from "@src/modules/overview/hooks/useVisibleCenterAnchor"
import { type ReactNode, type Ref, useCallback, useMemo, useRef } from "react"
import { TransitionGroup } from "react-transition-group"

const CARD_TRANSITION_MS = 600
const CARD_REORDER_MS = 300

/**
 * Previews all connected top-level devices side by side, in the order arranged by the user. Fills its parent, with
 * the cards centered in it, scrolling horizontally when they don't fit, also by the scroll wheel.
 *
 * The overlay insets are the space at the edges that is covered by things on top of the list. The cards are centered
 * in the space that remains, and can be scrolled out from under them. Changes to them are animated like a collapse,
 * so that the cards move along with an overlay that opens or closes, keeping the cards that were in view in view.
 */
export const DevicePreviewList = (props: { overlayInsets?: { right?: number, bottom?: number } }) => {
  const { right = 0, bottom = 0 } = props.overlayInsets ?? {}

  const connectedDevicesQuery = useConnectedDevices()

  const topLevelDevices = useMemo(
    () => (connectedDevicesQuery.data ?? []).filter(it => it.parentDeviceId === null),
    [connectedDevicesQuery.data],
  )
  const { orderedDevices, moveDevice } = useDeviceOrder(topLevelDevices)

  const reorderAnimation = useReorderAnimation(orderedDevices.map(it => it.deviceId), CARD_REORDER_MS)
  const { capturePositions } = reorderAnimation

  const theme = useTheme()
  const insetDurationMs = theme.transitions.duration.standard
  const scrollerRef = useRef<HTMLDivElement>(null)
  const cardsRef = useRef<HTMLDivElement>(null)
  useVisibleCenterAnchor(scrollerRef, cardsRef, right, insetDurationMs)
  useHorizontalWheelScroll(scrollerRef)

  const handleMove = useCallback((deviceId: string, offset: number) => {
    capturePositions()
    moveDevice(deviceId, offset)
  }, [capturePositions, moveDevice])

  return (
    <Box ref={scrollerRef} sx={{ width: "100%", height: "100%", overflow: "auto", display: "flex" }}>
      {/*
        Centered by auto margins rather than by justifying the content, as the margins collapse when the cards
        don't fit, so that all of them can still be scrolled to. Padded so the scroll area doesn't clip the shadows,
        and so the cards can be scrolled out from under the overlays.
      */}
      <Stack
        ref={cardsRef}
        direction={"row"}
        sx={{
          m: "auto",
          p: 0.5,
          pr: `${4 + right}px`,
          pb: `${4 + bottom}px`,
          alignItems: "flex-start",
          transition: theme.transitions.create("padding", { duration: insetDurationMs }),
        }}
      >
        <TransitionGroup component={null}>
          {
            // In the arranged order, so that the focus moves through the cards in the order they're shown. In strict
            // mode, the moved cards' transitions start over when they're reordered, as React runs the effects of
            // moved elements again. That only happens in development, and strict mode can be turned off to avoid it.
            orderedDevices.map((it, index) => (
              <CardTransition key={it.deviceId} rootRef={reorderAnimation.elementRef(it.deviceId)}>
                <TopLevelDevicePreviewCard
                  device={it}
                  action={
                    <>
                      <Tooltip title={"Move left"}>
                        <span>
                          <IconButton
                            size={"small"}
                            disabled={index === 0}
                            onClick={() => handleMove(it.deviceId, -1)}
                          >
                            <ChevronLeftIcon />
                          </IconButton>
                        </span>
                      </Tooltip>
                      <Tooltip title={"Move right"}>
                        <span>
                          <IconButton
                            size={"small"}
                            disabled={index === orderedDevices.length - 1}
                            onClick={() => handleMove(it.deviceId, 1)}
                          >
                            <ChevronRightIcon />
                          </IconButton>
                        </span>
                      </Tooltip>
                    </>
                  }
                />
              </CardTransition>
            ))
          }
        </TransitionGroup>
      </Stack>
    </Box>
  )
}

/**
 * Expands the card from its center when it's added, and contracts it towards its center when it's removed, fading it
 * in and out meanwhile. The space the card takes grows and shrinks with it, so that the other cards make room for it.
 *
 * Receives the transition props from the transition group, which are passed on to the collapse. The spacing between
 * the cards is padding within the collapse, so that it grows and shrinks with the card. The root ref is the element
 * that is moved when the cards are reordered.
 */
const CardTransition = (props: {
  in?: boolean
  appear?: boolean
  rootRef?: Ref<HTMLDivElement>
  children: ReactNode
}) => {
  const { in: isIn, appear = true, rootRef, children, ...transitionProps } = props

  return (
    <Collapse
      {...transitionProps}
      ref={rootRef}
      in={isIn}
      appear={appear}
      orientation={"horizontal"}
      timeout={CARD_TRANSITION_MS}
      // Centers the card in the space it takes, rather than placing it at the start, so that it's cut off evenly
      // on both sides while the space is narrower than the card
      sx={{ display: "flex", justifyContent: "center", "& .MuiCollapse-wrapper": { flexShrink: 0 } }}
    >
      <Zoom in={isIn} appear={appear} timeout={CARD_TRANSITION_MS}>
        <Box>
          <Fade in={isIn} appear={appear} timeout={CARD_TRANSITION_MS}>
            <Box sx={{ px: 1 }}>
              {children}
            </Box>
          </Fade>
        </Box>
      </Zoom>
    </Collapse>
  )
}
