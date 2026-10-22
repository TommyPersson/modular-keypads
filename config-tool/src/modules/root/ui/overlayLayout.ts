// The sizes of the things shown on top of the page, so that the page and the other overlays can make room for them.
// Kept apart from the components, so that pages can use them without importing the components, which import the pages.

export const MAIN_CONTROL_BAR_HEIGHT = 48
export const MAIN_CONTROL_BAR_MARGIN = 16
// Room for the horizontal scrollbar of the page below the bar, which is a bit more than the scrollbar is high
const PAGE_SCROLLBAR_SPACE = 16
// Where the bottom of the bar is, from the bottom of the window
export const MAIN_CONTROL_BAR_BOTTOM = PAGE_SCROLLBAR_SPACE + MAIN_CONTROL_BAR_MARGIN

export const TOOL_PANE_NAV_BAR_ITEM_WIDTH = 72
export const TOOL_PANE_NAV_BAR_MARGIN = 16

export const TOOL_PANE_WIDTH = 700
export const TOOL_PANE_MARGIN = 16

// Below the devices on the overview page
export const KEY_BINDINGS_PANE_HEIGHT = 192
export const KEY_BINDINGS_PANE_MAX_WIDTH = 1000

// The space at the edges of the window that is covered while a device is connected, more when a tool pane is open.
// The same space as the nav bar is kept clear on the left while no tool pane is open, so that
// what's between the insets is centered in the window. Next to an open tool pane, it's centered in the space that
// remains instead, which would be too narrow otherwise.
export function overlayInsets(isToolPaneOpen: boolean) {
  const navBarWidth = TOOL_PANE_NAV_BAR_ITEM_WIDTH + 2 * TOOL_PANE_NAV_BAR_MARGIN
  return {
    left: isToolPaneOpen ? 0 : navBarWidth,
    // The nav bar width includes the margin between it and the pane
    right: navBarWidth + (isToolPaneOpen ? TOOL_PANE_WIDTH + TOOL_PANE_MARGIN : 0),
    bottom: MAIN_CONTROL_BAR_BOTTOM + MAIN_CONTROL_BAR_HEIGHT + MAIN_CONTROL_BAR_MARGIN,
  }
}
