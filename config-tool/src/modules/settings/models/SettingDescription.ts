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
    description: "Time to keep the modifiers of mouse macros held after the last such macro, so that fast scrolling "
      + "holds them once instead of tapping them for each step. Zero releases them right after each macro.",
    unit: "ms",
  },
}

export function getSettingDescription(key: string): SettingDescription {
  return SettingDescriptions[key] ?? { title: key, description: "" }
}
