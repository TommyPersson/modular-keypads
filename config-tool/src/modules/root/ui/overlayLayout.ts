// The sizes of the things shown on top of the page, so that the page and the other overlays can make room for them.
// Kept apart from the components, so that pages can use them without importing the components, which import the pages.

export const MAIN_CONTROL_BAR_HEIGHT = 48
export const MAIN_CONTROL_BAR_MARGIN = 16

export const TOOL_PANE_NAV_BAR_ITEM_WIDTH = 72
export const TOOL_PANE_NAV_BAR_MARGIN = 16

export const TOOL_PANE_WIDTH = 700
export const TOOL_PANE_MARGIN = 16

// The space at the edges of the window that is covered while a device is connected, more when a tool pane is open
export function overlayInsets(isToolPaneOpen: boolean) {
  const navBarWidth = TOOL_PANE_NAV_BAR_ITEM_WIDTH + 2 * TOOL_PANE_NAV_BAR_MARGIN
  return {
    // The nav bar width includes the margin between it and the pane
    right: navBarWidth + (isToolPaneOpen ? TOOL_PANE_WIDTH + TOOL_PANE_MARGIN : 0),
    bottom: MAIN_CONTROL_BAR_HEIGHT + 2 * MAIN_CONTROL_BAR_MARGIN,
  }
}
