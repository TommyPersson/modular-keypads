const STORAGE_KEY = "dev.strictMode"

/**
 * Whether the app is rendered in strict mode, which is on unless turned off with `?strictMode=off`. The choice is
 * remembered until it's turned on again with `?strictMode=on`, so that it survives reloads and navigation.
 *
 * Strict mode only has an effect in development. Turning it off shows how the app behaves in production, e.g. for
 * transitions, which start over in strict mode when their elements are moved.
 */
export function isStrictModeEnabled(): boolean {
  const requested = new URLSearchParams(window.location.search).get("strictMode")

  try {
    if (requested === "on" || requested === "off") {
      localStorage.setItem(STORAGE_KEY, requested)
    }
    return (requested ?? localStorage.getItem(STORAGE_KEY)) !== "off"
  } catch {
    // Without storage, only the request applies
    return requested !== "off"
  }
}
