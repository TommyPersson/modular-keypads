export type SettingDescription = {
  title: string
  description: string
  unit?: string
}

// Settings without a description are still listed, using their key as title
export const SettingDescriptions: Record<string, SettingDescription> = {
  "typing.delay": {
    title: "Typing delay",
    description: "Time to wait between characters typed by text macros. Increase it if applications drop characters.",
    unit: "ms",
  },
  "modifier.hold": {
    title: "Modifier hold time",
    description: "Time to keep the modifiers of mouse macros, and of shortcut macros that hold them without a hold time "
      + "of their own, held after the last such macro. Fast scrolling then holds them once instead of tapping them for each step, and repeated "
      + "Alt + Tab keeps the window switcher open. Zero releases them right after each macro.",
    unit: "ms",
  },
}

export function getSettingDescription(key: string): SettingDescription {
  return SettingDescriptions[key] ?? { title: key, description: "" }
}
